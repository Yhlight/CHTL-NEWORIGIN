#include "../third-party/catch.hpp"
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLLoader/CHTLLoader.h"
#include "../CHTL/CHTLNode/ElementNode.h"
#include "../CHTL/CHTLNode/CustomNode.h"
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

// Helper to parse input string into an AST
std::shared_ptr<CHTL::BaseNode> parseString(const std::string& input) {
    CHTL::CHTLLexer lexer(input);
    std::vector<CHTL::Token> tokens;
    CHTL::Token token = lexer.getNextToken();
    while (token.type != CHTL::TokenType::TOKEN_EOF) {
        tokens.push_back(token);
        token = lexer.getNextToken();
    }
    CHTL::CHTLParser parser(tokens);
    return parser.parse();
}

// Test fixture for loader tests
struct LoaderTestFixture {
    LoaderTestFixture() {
        fs::create_directory("modules");
        fs::create_directory("module");
    }

    ~LoaderTestFixture() {
        if (fs::exists("modules")) fs::remove_all("modules");
        if (fs::exists("module")) fs::remove_all("module");
        if (fs::exists("test.html")) fs::remove("test.html");
        if (fs::exists("test.css")) fs::remove("test.css");
        if (fs::exists("test.js")) fs::remove("test.js");
        if (fs::exists("component.chtl")) fs::remove("component.chtl");
        if (fs::exists("namespaced.chtl")) fs::remove("namespaced.chtl");
        if (fs::exists("default_namespaced.chtl")) fs::remove("default_namespaced.chtl");
        if (fs::exists("test_import.chtl")) fs::remove("test_import.chtl");
    }
};

TEST_CASE_METHOD(LoaderTestFixture, "Loader handles a simple import", "[loader]") {
    std::ofstream outfile("test_import.chtl");
    outfile << "div { text { \"imported\" } }";
    outfile.close();

    std::string mainInput = R"([Import] @Chtl from "test_import.chtl";)";
    auto mainAst = parseString(mainInput);

    CHTL::CHTLLoader loader(".");
    loader.loadImports(mainAst);

    auto loadedAsts = loader.getLoadedAsts();
    REQUIRE(loadedAsts.size() == 1);

    auto importedAst = loadedAsts.begin()->second;
    REQUIRE(importedAst != nullptr);
    REQUIRE(importedAst->getChildren().size() == 1);
    auto importedNode = importedAst->getChildren()[0];
    REQUIRE(importedNode->getType() == CHTL::NodeType::NODE_ELEMENT);

    auto element = std::dynamic_pointer_cast<CHTL::ElementNode>(importedNode);
    REQUIRE(element->getTagName() == "div");
}

TEST_CASE_METHOD(LoaderTestFixture, "Loader resolves path from base directory", "[loader]") {
    std::ofstream outfile("component.chtl");
    outfile << "[Custom] @Element MyComponent {}";
    outfile.close();

    std::string mainInput = R"([Import] [Custom] @Element MyComponent from "component";)";
    auto mainAst = parseString(mainInput);
    CHTL::CHTLLoader loader(".");
    loader.loadImports(mainAst);

    REQUIRE(loader.getImportedItems().count("MyComponent") == 1);
}

TEST_CASE_METHOD(LoaderTestFixture, "Loader resolves path from local module directory", "[loader]") {
    std::ofstream outfile("module/component.chtl");
    outfile << "[Custom] @Element MyComponent {}";
    outfile.close();

    std::string mainInput = R"([Import] [Custom] @Element MyComponent from "component";)";
    auto mainAst = parseString(mainInput);
    CHTL::CHTLLoader loader(".");
    loader.loadImports(mainAst);

    REQUIRE(loader.getImportedItems().count("MyComponent") == 1);
}

TEST_CASE_METHOD(LoaderTestFixture, "Loader resolves path from official modules directory", "[loader]") {
    std::ofstream outfile("modules/component.chtl");
    outfile << "[Custom] @Element MyComponent {}";
    outfile.close();

    std::string mainInput = R"([Import] [Custom] @Element MyComponent from "component";)";
    auto mainAst = parseString(mainInput);
    CHTL::CHTLLoader loader(".");
    loader.loadImports(mainAst);

    REQUIRE(loader.getImportedItems().count("MyComponent") == 1);
}


TEST_CASE_METHOD(LoaderTestFixture, "Loader handles HTML import", "[loader]") {
    std::ofstream outfile("test.html");
    outfile << "<h1>Hello</h1>";
    outfile.close();

    std::string mainInput = R"([Import] @Html from "test.html" as myHtml;)";
    auto mainAst = parseString(mainInput);
    CHTL::CHTLLoader loader(".");
    loader.loadImports(mainAst);

    auto originNodes = loader.getNamedOriginNodes();
    REQUIRE(originNodes.size() == 1);
    REQUIRE(originNodes.count("myHtml") == 1);
    auto node = originNodes.at("myHtml");
    REQUIRE(node->getOriginType() == "@Html");
    REQUIRE(node->getContent() == "<h1>Hello</h1>");
}

TEST_CASE_METHOD(LoaderTestFixture, "Loader handles CSS import", "[loader]") {
    std::ofstream outfile("test.css");
    outfile << "body { color: blue; }";
    outfile.close();

    std::string mainInput = R"([Import] @Style from "test.css" as myCss;)";
    auto mainAst = parseString(mainInput);
    CHTL::CHTLLoader loader(".");
    loader.loadImports(mainAst);

    auto originNodes = loader.getNamedOriginNodes();
    REQUIRE(originNodes.size() == 1);
    REQUIRE(originNodes.count("myCss") == 1);
    auto node = originNodes.at("myCss");
    REQUIRE(node->getOriginType() == "@Style");
    REQUIRE(node->getContent() == "body { color: blue; }");
}

TEST_CASE_METHOD(LoaderTestFixture, "Loader handles JS import", "[loader]") {
    std::ofstream outfile("test.js");
    outfile << "console.log('test');";
    outfile.close();

    std::string mainInput = R"([Import] @JavaScript from "test.js" as myJs;)";
    auto mainAst = parseString(mainInput);
    CHTL::CHTLLoader loader(".");
    loader.loadImports(mainAst);

    auto originNodes = loader.getNamedOriginNodes();
    REQUIRE(originNodes.size() == 1);
    REQUIRE(originNodes.count("myJs") == 1);
    auto node = originNodes.at("myJs");
    REQUIRE(node->getOriginType() == "@JavaScript");
    REQUIRE(node->getContent() == "console.log('test');");
}


TEST_CASE_METHOD(LoaderTestFixture, "Loader handles precise import", "[loader]") {
    std::ofstream outfile("component.chtl");
    outfile << "[Custom] @Element MyComponent {}";
    outfile.close();

    std::string mainInput = R"([Import] [Custom] @Element MyComponent from "component.chtl";)";
    auto mainAst = parseString(mainInput);
    CHTL::CHTLLoader loader(".");
    loader.loadImports(mainAst);

    auto importedItems = loader.getImportedItems();
    REQUIRE(importedItems.size() == 1);
    REQUIRE(importedItems.count("MyComponent") == 1);
    auto node = importedItems.at("MyComponent");
    REQUIRE(node->getType() == CHTL::NodeType::NODE_CUSTOM);
    auto customNode = std::dynamic_pointer_cast<CHTL::CustomNode>(node);
    REQUIRE(customNode->getName() == "MyComponent");
}

TEST_CASE_METHOD(LoaderTestFixture, "Loader handles type-based import", "[loader]") {
    std::ofstream outfile("component.chtl");
    outfile << "[Custom] @Element ComponentA {} [Custom] @Element ComponentB {}";
    outfile.close();

    std::string mainInput = R"([Import] [Custom] @Element from "component.chtl";)";
    auto mainAst = parseString(mainInput);
    CHTL::CHTLLoader loader(".");
    loader.loadImports(mainAst);

    auto importedItems = loader.getImportedItems();
    REQUIRE(importedItems.size() == 2);
    REQUIRE(importedItems.count("ComponentA") == 1);
    REQUIRE(importedItems.count("ComponentB") == 1);
}

TEST_CASE_METHOD(LoaderTestFixture, "Loader handles namespace import", "[loader]") {
    std::ofstream outfile("namespaced.chtl");
    outfile << "[Namespace] MyNS { [Custom] @Element MyComponent {} }";
    outfile.close();

    std::string mainInput = R"([Import] @Chtl from "namespaced.chtl";)";
    auto mainAst = parseString(mainInput);
    CHTL::CHTLLoader loader(".");
    loader.loadImports(mainAst);

    auto namespaces = loader.getNamespaces();
    REQUIRE(namespaces.size() == 1);
    REQUIRE(namespaces.count("MyNS") == 1);
    auto ns = namespaces.at("MyNS");
    REQUIRE(ns.size() == 1);
    REQUIRE(ns.count("MyComponent") == 1);
}


TEST_CASE_METHOD(LoaderTestFixture, "Loader handles default namespace import", "[loader]") {
    std::ofstream outfile("default_namespaced.chtl");
    outfile << "[Custom] @Element MyComponent {}";
    outfile.close();

    std::string mainInput = R"([Import] @Chtl from "default_namespaced.chtl";)";
    auto mainAst = parseString(mainInput);
    CHTL::CHTLLoader loader(".");
    loader.loadImports(mainAst);

    auto namespaces = loader.getNamespaces();
    REQUIRE(namespaces.size() == 1);
    REQUIRE(namespaces.count("default_namespaced") == 1);
    auto ns = namespaces.at("default_namespaced");
    REQUIRE(ns.size() == 1);
    REQUIRE(ns.count("MyComponent") == 1);
}