/**
 * @file test_lexer.cpp
 * @brief Tests for CHTL Lexer
 */

#include "catch.hpp"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLLexer/Token.h"

using namespace CHTL;

TEST_CASE("Lexer tokenizes basic elements", "[lexer]") {
    SECTION("Single identifier") {
        Lexer lexer("div");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 2); // identifier + EOF
        REQUIRE(tokens[0].type == TokenType::IDENTIFIER);
        REQUIRE(tokens[0].lexeme == "div");
    }
    
    SECTION("Element with braces") {
        Lexer lexer("div { }");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 4); // div + { + } + EOF
        REQUIRE(tokens[0].type == TokenType::IDENTIFIER);
        REQUIRE(tokens[1].type == TokenType::LEFT_BRACE);
        REQUIRE(tokens[2].type == TokenType::RIGHT_BRACE);
    }
}

TEST_CASE("Lexer handles strings", "[lexer]") {
    SECTION("Double quoted string") {
        Lexer lexer("\"hello world\"");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 2); // string + EOF
        REQUIRE(tokens[0].type == TokenType::STRING_LITERAL);
        REQUIRE(tokens[0].lexeme == "hello world");
    }
    
    SECTION("Single quoted string") {
        Lexer lexer("'hello world'");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 2); // string + EOF
        REQUIRE(tokens[0].type == TokenType::STRING_LITERAL);
        REQUIRE(tokens[0].lexeme == "hello world");
    }
}

TEST_CASE("Lexer handles numbers", "[lexer]") {
    SECTION("Integer") {
        Lexer lexer("123");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 2); // number + EOF
        REQUIRE(tokens[0].type == TokenType::NUMBER);
        REQUIRE(tokens[0].lexeme == "123");
    }
    
    SECTION("Decimal") {
        Lexer lexer("123.45");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 2); // number + EOF
        REQUIRE(tokens[0].type == TokenType::NUMBER);
        REQUIRE(tokens[0].lexeme == "123.45");
    }
}

TEST_CASE("Lexer handles keywords", "[lexer]") {
    Lexer lexer("text style script");
    auto tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 4); // 3 keywords + EOF
    REQUIRE(tokens[0].type == TokenType::KEYWORD_TEXT);
    REQUIRE(tokens[1].type == TokenType::KEYWORD_STYLE);
    REQUIRE(tokens[2].type == TokenType::KEYWORD_SCRIPT);
}
