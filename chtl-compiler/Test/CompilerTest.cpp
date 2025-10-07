#define CATCH_CONFIG_MAIN
#include "third-party/catch.hpp"
#include "chtl-compiler/CHTL/CHTLLexer/Lexer.h"
#include "chtl-compiler/CHTL/CHTLParser/Parser.h"
#include "chtl-compiler/CHTL/CHTLNode/ElementNode.h"
#include "chtl-compiler/CHTL/CHTLNode/TextNode.h"

TEST_CASE("New Compiler - Lexer", "[new_compiler_lexer]") {
    SECTION("Tokenizes a simple structure") {
        std::string source = "div { text { \"Hello\" } }";
        Lexer lexer(source);

        Token t1 = lexer.getNextToken();
        REQUIRE(t1.type == TokenType::Identifier);
        REQUIRE(t1.value == "div");

        Token t2 = lexer.getNextToken();
        REQUIRE(t2.type == TokenType::LeftBrace);

        Token t3 = lexer.getNextToken();
        REQUIRE(t3.type == TokenType::Identifier);
        REQUIRE(t3.value == "text");

        Token t4 = lexer.getNextToken();
        REQUIRE(t4.type == TokenType::LeftBrace);

        Token t5 = lexer.getNextToken();
        REQUIRE(t5.type == TokenType::StringLiteral);
        REQUIRE(t5.value == "Hello");

        Token t6 = lexer.getNextToken();
        REQUIRE(t6.type == TokenType::RightBrace);

        Token t7 = lexer.getNextToken();
        REQUIRE(t7.type == TokenType::RightBrace);

        Token t8 = lexer.getNextToken();
        REQUIRE(t8.type == TokenType::EndOfFile);
    }
}

TEST_CASE("New Compiler - Parser", "[new_compiler_parser]") {
    SECTION("Parses a simple element") {
        std::string source = "div { }";
        Lexer lexer(source);
        Parser parser(lexer);
        NodeList nodes = parser.parse();

        REQUIRE(nodes.size() == 1);
        auto element = std::dynamic_pointer_cast<ElementNode>(nodes[0]);
        REQUIRE(element != nullptr);
        REQUIRE(element->tagName == "div");
        REQUIRE(element->children.empty());
    }

    SECTION("Parses a nested element") {
        std::string source = "div { p { } }";
        Lexer lexer(source);
        Parser parser(lexer);
        NodeList nodes = parser.parse();

        REQUIRE(nodes.size() == 1);
        auto parent = std::dynamic_pointer_cast<ElementNode>(nodes[0]);
        REQUIRE(parent != nullptr);
        REQUIRE(parent->tagName == "div");
        REQUIRE(parent->children.size() == 1);

        auto child = std::dynamic_pointer_cast<ElementNode>(parent->children[0]);
        REQUIRE(child != nullptr);
        REQUIRE(child->tagName == "p");
    }

    SECTION("Parses an element with text") {
        std::string source = "p { text { \"hello\" } }";
        Lexer lexer(source);
        Parser parser(lexer);
        NodeList nodes = parser.parse();

        REQUIRE(nodes.size() == 1);
        auto parent = std::dynamic_pointer_cast<ElementNode>(nodes[0]);
        REQUIRE(parent != nullptr);
        REQUIRE(parent->tagName == "p");
        REQUIRE(parent->children.size() == 1);

        auto child = std::dynamic_pointer_cast<TextNode>(parent->children[0]);
        REQUIRE(child != nullptr);
        REQUIRE(child->content == "hello");
    }

    SECTION("Parses multiple root elements") {
        std::string source = "p { } div { }";
        Lexer lexer(source);
        Parser parser(lexer);
        NodeList nodes = parser.parse();

        REQUIRE(nodes.size() == 2);
        auto p_element = std::dynamic_pointer_cast<ElementNode>(nodes[0]);
        REQUIRE(p_element != nullptr);
        REQUIRE(p_element->tagName == "p");

        auto div_element = std::dynamic_pointer_cast<ElementNode>(nodes[1]);
        REQUIRE(div_element != nullptr);
        REQUIRE(div_element->tagName == "div");
    }
}