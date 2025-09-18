#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLNode/Node.h"
#include "../CHTL/CHTLNode/ElementNode.h"
#include "../CHTL/CHTLNode/TextNode.h"
#include <iostream>
#include <cassert>

void checkParserErrors(const Parser& p) {
    const auto& errors = p.errors();
    if (errors.empty()) {
        return;
    }

    std::cerr << "Parser has " << errors.size() << " errors:" << std::endl;
    for (const auto& msg : errors) {
        std::cerr << "  - " << msg << std::endl;
    }
    assert(errors.empty());
}

void testSimpleElementParsing() {
    std::cout << "  Testing Simple Element and Text Parsing..." << std::endl;

    std::string input = R"(
html {
    body {
        text { "Hello CHTL" }
    }
}
)";

    Lexer l(input);
    Parser p(l);
    NodePtr program = p.parseProgram();

    checkParserErrors(p);
    assert(program != nullptr);

    auto root = std::dynamic_pointer_cast<ElementNode>(program);
    assert(root != nullptr && "Program root is not an ElementNode");
    assert(root->m_children.size() == 1 && "Program should have one child node");

    auto htmlNode = std::dynamic_pointer_cast<ElementNode>(root->m_children[0]);
    assert(htmlNode != nullptr && "First child is not an ElementNode");
    assert(htmlNode->m_tagName == "html" && "Tag name is not 'html'");
    assert(htmlNode->m_children.size() == 1 && "html should have one child");

    auto bodyNode = std::dynamic_pointer_cast<ElementNode>(htmlNode->m_children[0]);
    assert(bodyNode != nullptr && "Child of html is not an ElementNode");
    assert(bodyNode->m_tagName == "body" && "Tag name is not 'body'");
    assert(bodyNode->m_children.size() == 1 && "body should have one child");

    auto textElementNode = std::dynamic_pointer_cast<ElementNode>(bodyNode->m_children[0]);
    assert(textElementNode != nullptr && "Child of body is not an ElementNode");
    assert(textElementNode->m_tagName == "text" && "Tag name is not 'text'");
    assert(textElementNode->m_children.size() == 1 && "text element should have one child");

    auto textNode = std::dynamic_pointer_cast<TextNode>(textElementNode->m_children[0]);
    assert(textNode != nullptr && "Child of text element is not a TextNode");
    assert(textNode->m_value == "Hello CHTL" && "Text node value is incorrect");

    std::cout << "  ...Passed" << std::endl;
}

void RunParserTests() {
    std::cout << "--- Running Parser Tests ---" << std::endl;
    testSimpleElementParsing();
    std::cout << "---------------------------" << std::endl;
}
