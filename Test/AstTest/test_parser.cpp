#include "CHTLParser/Parser.h"
#include "CHTLLexer/Lexer.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h" // <-- New include
#include "CHTLContext/Context.h"
#include <iostream>
#include <cassert>
#include <memory>

void testElementAndAttributeParsing() {
    std::cout << "Running test: testElementAndAttributeParsing..." << std::endl;
    std::string source = "html { lang: \"en\"; body { class: main; } }";

    CHTL::Lexer lexer(source);
    auto tokens = lexer.tokenizeAll();
    auto context = std::make_shared<CHTL::Context>();
    CHTL::Parser parser(tokens, context);
    auto astRoot = parser.parse();

    assert(astRoot != nullptr);
    const auto& rootChildren = astRoot->getChildren();
    assert(rootChildren.size() == 1);

    auto htmlNode = std::dynamic_pointer_cast<CHTL::ElementNode>(rootChildren[0]);
    assert(htmlNode != nullptr);
    assert(htmlNode->tagName == "html");
    assert(htmlNode->attributes.size() == 1);
    assert(htmlNode->attributes.at("lang") == "\"en\"");

    const auto& htmlChildren = htmlNode->getChildren();
    assert(htmlChildren.size() == 1);

    auto bodyNode = std::dynamic_pointer_cast<CHTL::ElementNode>(htmlChildren[0]);
    assert(bodyNode != nullptr);
    assert(bodyNode->tagName == "body");
    assert(bodyNode->attributes.size() == 1);
    assert(bodyNode->attributes.at("class") == "main");
    assert(bodyNode->getChildren().empty());

    std::cout << "PASSED!" << std::endl;
}

void testTextNodeParsing() {
    std::cout << "Running test: testTextNodeParsing..." << std::endl;
    std::string source = "div { text: \"Hello\"; p { text { World } } }";

    CHTL::Lexer lexer(source);
    auto tokens = lexer.tokenizeAll();
    auto context = std::make_shared<CHTL::Context>();
    CHTL::Parser parser(tokens, context);
    auto astRoot = parser.parse();

    assert(astRoot != nullptr);
    const auto& rootChildren = astRoot->getChildren();
    assert(rootChildren.size() == 1);

    // Check the <div> node
    auto divNode = std::dynamic_pointer_cast<CHTL::ElementNode>(rootChildren[0]);
    assert(divNode != nullptr);
    assert(divNode->tagName == "div");
    assert(divNode->getChildren().size() == 2); // Should have a TextNode and an ElementNode

    // Check the first child (TextNode from attribute)
    auto textNode1 = std::dynamic_pointer_cast<CHTL::TextNode>(divNode->getChildren()[0]);
    assert(textNode1 != nullptr);
    assert(textNode1->content == "Hello");

    // Check the second child (p element)
    auto pNode = std::dynamic_pointer_cast<CHTL::ElementNode>(divNode->getChildren()[1]);
    assert(pNode != nullptr);
    assert(pNode->tagName == "p");
    assert(pNode->getChildren().size() == 1);

    // Check the p element's child (TextNode from block)
    auto textNode2 = std::dynamic_pointer_cast<CHTL::TextNode>(pNode->getChildren()[0]);
    assert(textNode2 != nullptr);
    assert(textNode2->content == "World");

    std::cout << "PASSED!" << std::endl;
}

int main() {
    testElementAndAttributeParsing();
    testTextNodeParsing(); // <-- Call new test
    std::cout << "All AstTest tests passed!" << std::endl;
    return 0;
}
