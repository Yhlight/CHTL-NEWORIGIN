#include "gtest/gtest.h"
#include "../src/Util/ImportResolver/ImportResolver.h"
#include "../src/CHTL/CHTLParser/CHTLParser.h"
#include "../src/CHTL/CHTLNode/RootNode.h"
#include "../src/CHTL/CHTLNode/ImportNode.h"
#include "../src/CHTL/CHTLNode/OriginNode.h"
#include "../src/CHTL/CHTLNode/ElementNode.h"
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
    ASSERT_EQ(root->getChildren()[0]->getType(), NodeType::Element);
    ElementNode* div = static_cast<ElementNode*>(root->getChildren()[0].get());
    ASSERT_EQ(div->getTagName(), "div");

    remove("imported.chtl");
}

TEST(ImportResolverTest, ResolvesHtmlImport) {
    std::string htmlContent = "<h1>Hello World</h1>";
    std::ofstream imported_file("imported.html");
    imported_file << htmlContent;
    imported_file.close();

    std::string main_content = R"([Import] @Html from "imported.html";)";

    CHTLParser parser(main_content);
    parser.parse();

    ImportResolver resolver;
    resolver.resolve(*parser.getRoot());

    BaseNode* root = parser.getRoot();
    ASSERT_EQ(root->getChildren().size(), 1);
    ASSERT_EQ(root->getChildren()[0]->getType(), NodeType::Origin);

    OriginNode* origin = static_cast<OriginNode*>(root->getChildren()[0].get());
    ASSERT_EQ(origin->getOriginType(), "Html");
    ASSERT_EQ(origin->getContent(), htmlContent);

    remove("imported.html");
}

TEST(ImportResolverTest, ResolvesCssImport) {
    std::string cssContent = "body { color: red; }";
    std::ofstream imported_file("imported.css");
    imported_file << cssContent;
    imported_file.close();

    std::string main_content = R"([Import] @Style from "imported.css";)";

    CHTLParser parser(main_content);
    parser.parse();

    ImportResolver resolver;
    resolver.resolve(*parser.getRoot());

    BaseNode* root = parser.getRoot();
    ASSERT_EQ(root->getChildren().size(), 1);
    ASSERT_EQ(root->getChildren()[0]->getType(), NodeType::Origin);

    OriginNode* origin = static_cast<OriginNode*>(root->getChildren()[0].get());
    ASSERT_EQ(origin->getOriginType(), "Style");
    ASSERT_EQ(origin->getContent(), cssContent);

    remove("imported.css");
}