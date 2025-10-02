#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLNode/ElementNode.h"
#include "../CHTL/CHTLNode/TextNode.h"
#include "../CHTL/CHTLNode/AttributeNode.h"
#include "Assert.h"
#include <iostream>


void testSimpleElement() {
    std::cout << "Running testSimpleElement..." << std::endl;
    std::string source = "div {}";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto root = parser.parse();

    CHTL::Test::assertEquals(1, (int)root->children.size(), "testSimpleElement: Root should have one child");
    ElementNode* divNode = dynamic_cast<ElementNode*>(root->children[0].get());
    CHTL::Test::assertEquals(true, divNode != nullptr, "testSimpleElement: Child should be an ElementNode");
    CHTL::Test::assertEquals("div", divNode->tagName, "testSimpleElement: Element tag name");
}

void testTextNode() {
    std::cout << "Running testTextNode..." << std::endl;
    std::string source = "text { \"hello\" }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto root = parser.parse();

    CHTL::Test::assertEquals(1, (int)root->children.size(), "testTextNode: Root should have one child");
    TextNode* textNode = dynamic_cast<TextNode*>(root->children[0].get());
    CHTL::Test::assertEquals(true, textNode != nullptr, "testTextNode: Child should be a TextNode");
    CHTL::Test::assertEquals("hello", textNode->content, "testTextNode: Text content");
}


void testElementWithAttributes() {
    std::cout << "Running testElementWithAttributes..." << std::endl;
    std::string source = "div { id: main; }"; // Use unquoted literal
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto root = parser.parse();

    CHTL::Test::assertEquals(1, (int)root->children.size(), "testElementWithAttributes: Root should have one child");
    ElementNode* divNode = dynamic_cast<ElementNode*>(root->children[0].get());
    CHTL::Test::assertEquals(true, divNode != nullptr, "testElementWithAttributes: Child should be an ElementNode");
    CHTL::Test::assertEquals(1, (int)divNode->attributes.size(), "testElementWithAttributes: Should have one attribute");
    CHTL::Test::assertEquals("id", divNode->attributes[0]->name, "testElementWithAttributes: Attribute name");
    CHTL::Test::assertEquals("main", divNode->attributes[0]->value, "testElementWithAttributes: Attribute value");
}


void testNestedElements() {
    std::cout << "Running testNestedElements..." << std::endl;
    std::string source = "body { h1 { text { \"Title\" } } }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto root = parser.parse();

    // root -> body
    CHTL::Test::assertEquals(1, (int)root->children.size(), "testNestedElements: Root should have one child (body)");
    ElementNode* bodyNode = dynamic_cast<ElementNode*>(root->children[0].get());
    CHTL::Test::assertEquals("body", bodyNode->tagName, "testNestedElements: body tag name");

    // body -> h1
    CHTL::Test::assertEquals(1, (int)bodyNode->children.size(), "testNestedElements: Body should have one child (h1)");
    ElementNode* h1Node = dynamic_cast<ElementNode*>(bodyNode->children[0].get());
    CHTL::Test::assertEquals("h1", h1Node->tagName, "testNestedElements: h1 tag name");

    // h1 -> text
    CHTL::Test::assertEquals(1, (int)h1Node->children.size(), "testNestedElements: h1 should have one child (text)");
    TextNode* textNode = dynamic_cast<TextNode*>(h1Node->children[0].get());
    CHTL::Test::assertEquals(true, textNode != nullptr, "testNestedElements: h1 child should be TextNode");
    CHTL::Test::assertEquals("Title", textNode->content, "testNestedElements: text content");
}


int main() {
    try {
        testSimpleElement();
        testTextNode();
        testElementWithAttributes();
        testNestedElements();
        std::cout << "All parser tests passed!" << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << "Parser test failed: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}