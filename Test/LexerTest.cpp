#include "catch.hpp"
#include "CHTL/CHTL/CHTLLexer/Lexer.h"
#include <vector>

using namespace CHTL;

void checkTokens(const std::string& source, const std::vector<TokenType>& expected_types) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    REQUIRE(tokens.size() == expected_types.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        REQUIRE(tokens[i].type == expected_types[i]);
    }
}

TEST_CASE("Lexer correctly scans single-character tokens") {
    std::string source = "{}()[]:;,.?@#$=%+-*";
    std::vector<TokenType> expected = {
        TokenType::LEFT_BRACE, TokenType::RIGHT_BRACE,
        TokenType::LEFT_PAREN, TokenType::RIGHT_PAREN,
        TokenType::LEFT_BRACKET, TokenType::RIGHT_BRACKET,
        TokenType::COLON, TokenType::SEMICOLON, TokenType::COMMA, TokenType::DOT,
        TokenType::QUESTION, TokenType::AT, TokenType::HASH, TokenType::DOLLAR,
        TokenType::EQUAL, TokenType::PERCENT, TokenType::PLUS, TokenType::MINUS,
        TokenType::STAR,
        TokenType::END_OF_FILE
    };
    checkTokens(source, expected);
}

TEST_CASE("Lexer correctly scans multi-character tokens") {
    std::string source = "** && || -> &-> {{ }}";
    std::vector<TokenType> expected = {
        TokenType::STAR_STAR,
        TokenType::AMPERSAND_AMPERSAND,
        TokenType::PIPE_PIPE,
        TokenType::ARROW,
        TokenType::AMPERSAND_ARROW,
        TokenType::LEFT_DOUBLE_BRACE,
        TokenType::RIGHT_DOUBLE_BRACE,
        TokenType::END_OF_FILE
    };
    checkTokens(source, expected);
}

TEST_CASE("Lexer correctly scans comments") {
    std::string source = R"(
        // This is a line comment.
        div {}
        /* This is a block comment.
           It can span multiple lines.
           /* Nested block comment */
        */
        span {}
    )";
    std::vector<TokenType> expected = {
        TokenType::IDENTIFIER, TokenType::LEFT_BRACE, TokenType::RIGHT_BRACE,
        TokenType::IDENTIFIER, TokenType::LEFT_BRACE, TokenType::RIGHT_BRACE,
        TokenType::END_OF_FILE
    };
    checkTokens(source, expected);
}

TEST_CASE("Lexer correctly scans string literals") {
    std::string source = R"(
        "this is a string"
        'this is also a string'
    )";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0].type == TokenType::STRING);
    REQUIRE(std::get<std::string>(tokens[0].literal) == "this is a string");
    REQUIRE(tokens[1].type == TokenType::STRING);
    REQUIRE(std::get<std::string>(tokens[1].literal) == "this is also a string");
    REQUIRE(tokens[2].type == TokenType::END_OF_FILE);
}

TEST_CASE("Lexer correctly scans number literals") {
    std::string source = "123 45.67 0.987";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    REQUIRE(tokens.size() == 4);
    REQUIRE(tokens[0].type == TokenType::NUMBER);
    REQUIRE(std::get<double>(tokens[0].literal) == 123.0);
    REQUIRE(tokens[1].type == TokenType::NUMBER);
    REQUIRE(std::get<double>(tokens[1].literal) == 45.67);
    REQUIRE(tokens[2].type == TokenType::NUMBER);
    REQUIRE(std::get<double>(tokens[2].literal) == 0.987);
    REQUIRE(tokens[3].type == TokenType::END_OF_FILE);
}

TEST_CASE("Lexer correctly scans identifiers and keywords") {
    std::string source = "div text style script my_var";
    std::vector<TokenType> expected = {
        TokenType::IDENTIFIER,
        TokenType::TEXT,
        TokenType::STYLE,
        TokenType::SCRIPT,
        TokenType::IDENTIFIER,
        TokenType::END_OF_FILE
    };
    checkTokens(source, expected);
}

TEST_CASE("Lexer handles a simple CHTL block") {
    std::string source = R"(
        div {
            style {
                color: red;
            }
        }
    )";
    std::vector<TokenType> expected = {
        TokenType::IDENTIFIER, TokenType::LEFT_BRACE,
        TokenType::STYLE, TokenType::LEFT_BRACE,
        TokenType::IDENTIFIER, TokenType::COLON, TokenType::IDENTIFIER, TokenType::SEMICOLON,
        TokenType::RIGHT_BRACE,
        TokenType::RIGHT_BRACE,
        TokenType::END_OF_FILE
    };
    checkTokens(source, expected);
}