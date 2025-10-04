#include "gtest/gtest.h"
#include "../src/Util/ImportResolver/ImportResolver.h"
#include "../src/CHTL/CHTLParser/CHTLParser.h"
#include "../src/CHTL/CHTLNode/RootNode.h"
#include "../src/CHTL/CHTLNode/ImportNode.h"
#include "../src/CHTL/CHTLNode/OriginNode.h"
#include "../src/CHTL/CHTLNode/ElementNode.h"
#include "../src/CHTL/CHTLNode/NamespaceNode.h"
#include "../src/CHTL/CHTLNode/TemplateDefinitionNode.h"
#include <fstream>
#include <iostream>

TEST(ImportResolverTest, ResolvesSimpleChtlImport) {
    // Create a file to be imported
    std::ofstream imported_file("imported.chtl");
    imported_file << "div {}";
    imported_file.close();

    // Create the main file that imports the other file
    std::string main_content = R"([Import] @Chtl from "imported.chtl";)";

    CHTLParser parser(main_content);
    parser.parse();

    ImportResolver resolver;
    resolver.resolve(*parser.getRoot());

    BaseNode* root = parser.getRoot();
    ASSERT_EQ(root->getChildren().size(), 1);

    // The import should be wrapped in a default namespace
    ASSERT_EQ(root->getChildren()[0]->getType(), NodeType::Namespace);
    NamespaceNode* ns = static_cast<NamespaceNode*>(root->getChildren()[0].get());
    ASSERT_EQ(ns->getName(), "imported");

    // The namespace should contain the imported element
    ASSERT_EQ(ns->getChildren().size(), 1);
    ASSERT_EQ(ns->getChildren()[0]->getType(), NodeType::Element);
    ElementNode* div = static_cast<ElementNode*>(ns->getChildren()[0].get());
    ASSERT_EQ(div->getTagName(), "div");

    remove("imported.chtl");
}

TEST(ImportResolverTest, ResolvesHtmlImportWithAlias) {
    std::string htmlContent = "<h1>Hello World</h1>";
    std::ofstream imported_file("imported.html");
    imported_file << htmlContent;
    imported_file.close();

    std::string main_content = R"([Import] @Html from "imported.html" as myHtml;)";

    CHTLParser parser(main_content);
    parser.parse();

    ImportResolver resolver;
    resolver.resolve(*parser.getRoot());

    BaseNode* root = parser.getRoot();
    ASSERT_EQ(root->getChildren().size(), 1);
    ASSERT_EQ(root->getChildren()[0]->getType(), NodeType::Origin);

    OriginNode* origin = static_cast<OriginNode*>(root->getChildren()[0].get());
    ASSERT_EQ(origin->getOriginType(), "Html");
    ASSERT_EQ(origin->getName(), "myHtml");
    ASSERT_EQ(origin->getContent(), htmlContent);

    remove("imported.html");
}

TEST(ImportResolverTest, ResolvesCssImportWithAlias) {
    std::string cssContent = "body { color: red; }";
    std::ofstream imported_file("imported.css");
    imported_file << cssContent;
    imported_file.close();

    std::string main_content = R"([Import] @Style from "imported.css" as myStyle;)";

    CHTLParser parser(main_content);
    parser.parse();

    ImportResolver resolver;
    resolver.resolve(*parser.getRoot());

    BaseNode* root = parser.getRoot();
    ASSERT_EQ(root->getChildren().size(), 1);
    ASSERT_EQ(root->getChildren()[0]->getType(), NodeType::Origin);

    OriginNode* origin = static_cast<OriginNode*>(root->getChildren()[0].get());
    ASSERT_EQ(origin->getOriginType(), "Style");
    ASSERT_EQ(origin->getName(), "myStyle");
    ASSERT_EQ(origin->getContent(), cssContent);

    remove("imported.css");
}

TEST(ImportResolverTest, ResolvesJavaScriptImportWithAlias) {
    std::string jsContent = "console.log('Hello from imported file');";
    std::ofstream imported_file("imported.js");
    imported_file << jsContent;
    imported_file.close();

    std::string main_content = R"([Import] @JavaScript from "imported.js" as myScript;)";

    CHTLParser parser(main_content);
    parser.parse();

    ImportResolver resolver;
    resolver.resolve(*parser.getRoot());

    BaseNode* root = parser.getRoot();
    ASSERT_EQ(root->getChildren().size(), 1);
    ASSERT_EQ(root->getChildren()[0]->getType(), NodeType::Origin);

    OriginNode* origin = static_cast<OriginNode*>(root->getChildren()[0].get());
    ASSERT_EQ(origin->getOriginType(), "JavaScript");
    ASSERT_EQ(origin->getName(), "myScript");
    ASSERT_EQ(origin->getContent(), jsContent);

    remove("imported.js");
}

TEST(ImportResolverTest, ResolvesPreciseImport) {
    std::ofstream imported_file("defs.chtl");
    imported_file << R"(
        [Template] @Style MyStyle { color: blue; }
        [Template] @Element MyElement { div; }
    )";
    imported_file.close();

    std::string main_content = R"([Import] [Template] @Style MyStyle from "defs.chtl";)";

    CHTLParser parser(main_content);
    parser.parse();

    ImportResolver resolver;
    resolver.resolve(*parser.getRoot());

    BaseNode* root = parser.getRoot();
    ASSERT_EQ(root->getChildren().size(), 1);
    ASSERT_EQ(root->getChildren()[0]->getType(), NodeType::TemplateDefinition);

    TemplateDefinitionNode* def = static_cast<TemplateDefinitionNode*>(root->getChildren()[0].get());
    ASSERT_EQ(def->getCategory(), "[Template]");
    ASSERT_EQ(def->getTemplateType(), "Style");
    ASSERT_EQ(def->getTemplateName(), "MyStyle");

    remove("defs.chtl");
}

TEST(ImportResolverTest, RespectsExplicitNamespace) {
    std::ofstream imported_file("ns_test.chtl");
    imported_file << R"(
        [Namespace] my_app {
            div {}
        }
    )";
    imported_file.close();

    std::string main_content = R"([Import] @Chtl from "ns_test.chtl";)";

    CHTLParser parser(main_content);
    parser.parse();

    ImportResolver resolver;
    resolver.resolve(*parser.getRoot());

    BaseNode* root = parser.getRoot();
    ASSERT_EQ(root->getChildren().size(), 1);

    // The import should be wrapped in the explicit namespace
    ASSERT_EQ(root->getChildren()[0]->getType(), NodeType::Namespace);
    NamespaceNode* ns = static_cast<NamespaceNode*>(root->getChildren()[0].get());
    ASSERT_EQ(ns->getName(), "my_app");

    // The namespace should contain the element
    ASSERT_EQ(ns->getChildren().size(), 1);
    ASSERT_EQ(ns->getChildren()[0]->getType(), NodeType::Element);

    remove("ns_test.chtl");
}