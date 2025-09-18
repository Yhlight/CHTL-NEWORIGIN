#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLNode/Node.h"
#include "../CHTL/CHTLNode/ElementNode.h"
#include "../CHTL/CHTLNode/TextNode.h"
#include "../CHTL/CHTLNode/CommentNode.h"
#include <iostream>
#include <cassert>

void checkParserErrors(const Parser& p) {
    const auto& errors = p.errors();
    if (errors.empty()) {
        return;
    }

    std::cerr << "\n--- PARSER ERRORS ---" << std::endl;
    std::cerr << "Parser has " << errors.size() << " errors:" << std::endl;
    for (const auto& msg : errors) {
        std::cerr << "  - " << msg << std::endl;
    }
    std::cerr << "----------------------" << std::endl;
    assert(errors.empty());
}

void testAttributeParsing() {
    std::cout << "  Testing Attribute Parsing..." << std::endl;

    std::string input = R"(
div {
    id = "main";
    class: container;
}
)";

    Lexer l(input);
    Parser p(l);
    NodePtr program = p.parseProgram();
    checkParserErrors(p);

    auto root = std::dynamic_pointer_cast<ElementNode>(program);
    assert(root->m_children.size() == 1);

    auto divNode = std::dynamic_pointer_cast<ElementNode>(root->m_children[0]);
    assert(divNode != nullptr);
    assert(divNode->m_tagName == "div");

    assert(divNode->m_attributes.size() == 2);
    assert(divNode->m_attributes.count("id") == 1);
    assert(divNode->m_attributes.at("id") == "main");
    assert(divNode->m_attributes.count("class") == 1);
    assert(divNode->m_attributes.at("class") == "container");

    std::cout << "  ...Passed" << std::endl;
}

void testStyleBlockParsing() {
    std::cout << "  Testing Style Block Parsing..." << std::endl;
    std::string input = R"(
div {
    style {
        color: red;
        font-size: 16px;
    }
}
)";
    Lexer l(input);
    Parser p(l);
    NodePtr program = p.parseProgram();
    checkParserErrors(p);

    auto root = std::dynamic_pointer_cast<ElementNode>(program);
    assert(root->m_children.size() == 1);

    auto divNode = std::dynamic_pointer_cast<ElementNode>(root->m_children[0]);
    assert(divNode != nullptr);
    assert(divNode->m_tagName == "div");

    assert(divNode->m_inlineStyles.size() == 2);
    assert(divNode->m_inlineStyles.count("color") == 1);
    assert(divNode->m_inlineStyles.at("color") == "red");
    assert(divNode->m_inlineStyles.count("font-size") == 1);
    assert(divNode->m_inlineStyles.at("font-size") == "16px");

    std::cout << "  ...Passed" << std::endl;
}


void RunParserTests() {
    std::cout << "--- Running Parser Tests ---" << std::endl;
    testAttributeParsing();
    testStyleBlockParsing();
    std::cout << "---------------------------" << std::endl;
}
