#include "catch.hpp"
#include "CHTLLexer/CHTLLexer.h"
#include <vector>

TEST_CASE("Lexer Initialization", "[lexer]") {
    CHTL::CHTLLexer lexer;
    SUCCEED("Lexer can be initialized.");
}

TEST_CASE("Tokenize Single-Line Comment", "[lexer]") {
    CHTL::CHTLLexer lexer;
    std::string input = "// This is a test comment";
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == CHTL::TokenType::COMMENT);
    REQUIRE(tokens[0].value == " This is a test comment");
}

TEST_CASE("Tokenize Multi-Line Comment", "[lexer]") {
    CHTL::CHTLLexer lexer;
    std::string input = "/* This is a multi-line comment */";
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == CHTL::TokenType::COMMENT);
    REQUIRE(tokens[0].value == " This is a multi-line comment ");
}

TEST_CASE("Tokenize Generator Comment", "[lexer]") {
    CHTL::CHTLLexer lexer;
    std::string input = "# this is a generator comment";
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == CHTL::TokenType::COMMENT);
    // As per spec, the space after '#' is a delimiter and not part of the value.
    REQUIRE(tokens[0].value == "this is a generator comment");
}

TEST_CASE("Tokenize Text Block", "[lexer]") {
    CHTL::CHTLLexer lexer;
    std::string input = "text { \"Hello\" }";
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    REQUIRE(tokens.size() == 4);
    REQUIRE(tokens[0].type == CHTL::TokenType::KEYWORD_TEXT);
    REQUIRE(tokens[0].value == "text");
    REQUIRE(tokens[1].type == CHTL::TokenType::L_BRACE);
    REQUIRE(tokens[1].value == "{");
    REQUIRE(tokens[2].type == CHTL::TokenType::STRING_LITERAL);
    REQUIRE(tokens[2].value == "Hello");
    REQUIRE(tokens[3].type == CHTL::TokenType::R_BRACE);
    REQUIRE(tokens[3].value == "}");
}

TEST_CASE("Tokenize Attribute Syntax Characters", "[lexer]") {
    CHTL::CHTLLexer lexer;
    std::string input = ":=;";
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0].type == CHTL::TokenType::COLON);
    REQUIRE(tokens[1].type == CHTL::TokenType::EQUAL);
    REQUIRE(tokens[2].type == CHTL::TokenType::SEMICOLON);
}

TEST_CASE("Tokenize Number", "[lexer]") {
    CHTL::CHTLLexer lexer;
    std::string input = "123";
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == CHTL::TokenType::NUMBER);
    REQUIRE(tokens[0].value == "123");
}

TEST_CASE("Tokenize Script Keyword", "[lexer]") {
    CHTL::CHTLLexer lexer;
    std::string input = "script";
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == CHTL::TokenType::KEYWORD_SCRIPT);
    REQUIRE(tokens[0].value == "script");
}

TEST_CASE("Tokenize Arithmetic Operators", "[lexer]") {
    CHTL::CHTLLexer lexer;
    std::string input = "+ - * / % **";
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    REQUIRE(tokens.size() == 6);
    REQUIRE(tokens[0].type == CHTL::TokenType::PLUS);
    REQUIRE(tokens[1].type == CHTL::TokenType::MINUS);
    REQUIRE(tokens[2].type == CHTL::TokenType::STAR);
    REQUIRE(tokens[3].type == CHTL::TokenType::SLASH);
    REQUIRE(tokens[4].type == CHTL::TokenType::PERCENT);
    REQUIRE(tokens[5].type == CHTL::TokenType::STAR_STAR);
}

TEST_CASE("Tokenize Template Definition", "[lexer]") {
    CHTL::CHTLLexer lexer;
    std::string input = "[Template] @Style DefaultText";
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    REQUIRE(tokens.size() == 6);
    REQUIRE(tokens[0].type == CHTL::TokenType::L_BRACKET);
    REQUIRE(tokens[0].value == "[");
    REQUIRE(tokens[1].type == CHTL::TokenType::KEYWORD_TEMPLATE);
    REQUIRE(tokens[1].value == "Template");
    REQUIRE(tokens[2].type == CHTL::TokenType::R_BRACKET);
    REQUIRE(tokens[2].value == "]");
    REQUIRE(tokens[3].type == CHTL::TokenType::AT_SIGN);
    REQUIRE(tokens[3].value == "@");
    REQUIRE(tokens[4].type == CHTL::TokenType::KEYWORD_STYLE);
    REQUIRE(tokens[4].value == "Style");
    REQUIRE(tokens[5].type == CHTL::TokenType::IDENTIFIER);
    REQUIRE(tokens[5].value == "DefaultText");
}