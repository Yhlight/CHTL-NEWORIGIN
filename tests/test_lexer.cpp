#include <catch.hpp>
#include "CHTL/CHTLLexer.h"

TEST_CASE("CHTLLexer - Basic Tokenization", "[lexer]") {
    CHTL::CHTLLexer lexer;
    lexer.initialize();
    
    SECTION("Simple HTML element") {
        std::string input = "div { text: \"Hello World\"; }";
        auto tokens = lexer.tokenize(input);
        
        REQUIRE(tokens.size() > 0);
        REQUIRE(tokens[0].type == CHTL::TokenType::IDENTIFIER);
        REQUIRE(tokens[0].value == "div");
    }
    
    SECTION("Comments") {
        std::string input = "// This is a comment\ndiv { }";
        auto tokens = lexer.tokenize(input);
        
        REQUIRE(tokens.size() > 0);
        // Should skip comment and find div
        bool foundDiv = false;
        for (const auto& token : tokens) {
            if (token.type == CHTL::TokenType::IDENTIFIER && token.value == "div") {
                foundDiv = true;
                break;
            }
        }
        REQUIRE(foundDiv);
    }
    
    SECTION("String literals") {
        std::string input = "\"Hello World\"";
        auto tokens = lexer.tokenize(input);
        
        REQUIRE(tokens.size() >= 2); // String literal + EOF
        REQUIRE(tokens[0].type == CHTL::TokenType::STRING_LITERAL);
        REQUIRE(tokens[0].value == "Hello World");
    }
    
    SECTION("Keywords") {
        std::string input = "text style script";
        auto tokens = lexer.tokenize(input);
        
        REQUIRE(tokens.size() >= 4); // 3 keywords + EOF
        REQUIRE(tokens[0].type == CHTL::TokenType::TEXT);
        REQUIRE(tokens[1].type == CHTL::TokenType::STYLE);
        REQUIRE(tokens[2].type == CHTL::TokenType::SCRIPT);
    }
}

TEST_CASE("CHTLLexer - Complex Syntax", "[lexer]") {
    CHTL::CHTLLexer lexer;
    lexer.initialize();
    
    SECTION("Template syntax") {
        std::string input = "[Template] @Style DefaultText { color: \"black\"; }";
        auto tokens = lexer.tokenize(input);
        
        REQUIRE(tokens.size() > 0);
        bool foundTemplate = false;
        bool foundStyle = false;
        for (const auto& token : tokens) {
            if (token.type == CHTL::TokenType::TEMPLATE) {
                foundTemplate = true;
            } else if (token.type == CHTL::TokenType::STYLE) {
                foundStyle = true;
            }
        }
        REQUIRE(foundTemplate);
        REQUIRE(foundStyle);
    }
    
    SECTION("Custom syntax") {
        std::string input = "[Custom] @Element Box { div { } }";
        auto tokens = lexer.tokenize(input);
        
        REQUIRE(tokens.size() > 0);
        bool foundCustom = false;
        bool foundElement = false;
        for (const auto& token : tokens) {
            if (token.type == CHTL::TokenType::CUSTOM) {
                foundCustom = true;
            } else if (token.type == CHTL::TokenType::IDENTIFIER && token.value == "Element") {
                foundElement = true;
            }
        }
        REQUIRE(foundCustom);
        REQUIRE(foundElement);
    }
}

TEST_CASE("CHTLLexer - Error Handling", "[lexer]") {
    CHTL::CHTLLexer lexer;
    lexer.initialize();
    
    SECTION("Empty input") {
        std::string input = "";
        auto tokens = lexer.tokenize(input);
        
        REQUIRE(tokens.size() == 1); // Only EOF token
        REQUIRE(tokens[0].type == CHTL::TokenType::EOF_TOKEN);
    }
    
    SECTION("Whitespace only") {
        std::string input = "   \n\t  ";
        auto tokens = lexer.tokenize(input);
        
        REQUIRE(tokens.size() == 1); // Only EOF token
        REQUIRE(tokens[0].type == CHTL::TokenType::EOF_TOKEN);
    }
}