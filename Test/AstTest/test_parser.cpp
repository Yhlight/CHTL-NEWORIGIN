#include "CHTLParser/Parser.h"
#include "CHTLLexer/Lexer.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLContext/Context.h"
#include <iostream>
#include <cassert>
#include <memory>

void testElementAndAttributeParsing() {
    std::cout << "Running test: testElementAndAttributeParsing..." << std::endl;
    std::string source = "html { lang: \"en\"; body { class: main; } }";

    // 1. Lex the source code
    CHTL::Lexer lexer(source);
    auto tokens = lexer.tokenizeAll();

    // 2. Create context and parser
    auto context = std::make_shared<CHTL::Context>();
    CHTL::Parser parser(tokens, context);

    // 3. Parse and get the AST root
    auto astRoot = parser.parse();

    // 4. Assertions to verify the AST structure
    assert(astRoot != nullptr);

    // Check root's children (should have one child: the <html> element)
    const auto& rootChildren = astRoot->getChildren();
    assert(rootChildren.size() == 1);

    // Check the <html> node
    auto htmlNode = std::dynamic_pointer_cast<CHTL::ElementNode>(rootChildren[0]);
    assert(htmlNode != nullptr);
    assert(htmlNode->tagName == "html");
    assert(htmlNode->attributes.size() == 1);
    assert(htmlNode->attributes.count("lang") == 1);
    assert(htmlNode->attributes.at("lang") == "\"en\"");

    // Check <html>'s children (should have one child: the <body> element)
    const auto& htmlChildren = htmlNode->getChildren();
    assert(htmlChildren.size() == 1);

    // Check the <body> node
    auto bodyNode = std::dynamic_pointer_cast<CHTL::ElementNode>(htmlChildren[0]);
    assert(bodyNode != nullptr);
    assert(bodyNode->tagName == "body");
    assert(bodyNode->attributes.size() == 1);
    assert(bodyNode->attributes.count("class") == 1);
    assert(bodyNode->attributes.at("class") == "main");
    assert(bodyNode->getChildren().empty()); // Body should have no children in this test

    std::cout << "PASSED!" << std::endl;
}

int main() {
    testElementAndAttributeParsing();
    std::cout << "All AstTest tests passed!" << std::endl;
    return 0;
}
