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
    REQUIRE(tokens[0].type == CHTL::TokenType::TEXT_KEYWORD);
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
    REQUIRE(tokens[0].type == CHTL::TokenType::SCRIPT_KEYWORD);
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

TEST_CASE("Tokenize All New Keywords", "[lexer]") {
    CHTL::CHTLLexer lexer;
    std::string input = "use html5 inherit Custom delete insert after before replace at top bottom";
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    REQUIRE(tokens.size() == 12);
    REQUIRE(tokens[0].type == CHTL::TokenType::USE_KEYWORD);
    REQUIRE(tokens[1].type == CHTL::TokenType::HTML5_KEYWORD);
    REQUIRE(tokens[2].type == CHTL::TokenType::INHERIT_KEYWORD);
    REQUIRE(tokens[3].type == CHTL::TokenType::CUSTOM_KEYWORD);
    REQUIRE(tokens[4].type == CHTL::TokenType::DELETE_KEYWORD);
    REQUIRE(tokens[5].type == CHTL::TokenType::INSERT_KEYWORD);
    REQUIRE(tokens[6].type == CHTL::TokenType::AFTER_KEYWORD);
    REQUIRE(tokens[7].type == CHTL::TokenType::BEFORE_KEYWORD);
    REQUIRE(tokens[8].type == CHTL::TokenType::REPLACE_KEYWORD);
    REQUIRE(tokens[9].type == CHTL::TokenType::AT_KEYWORD);
    REQUIRE(tokens[10].type == CHTL::TokenType::TOP_KEYWORD);
    REQUIRE(tokens[11].type == CHTL::TokenType::BOTTOM_KEYWORD);
}

TEST_CASE("Tokenize Hex Literals", "[lexer]") {
    CHTL::CHTLLexer lexer;
    std::string input = "#fff #123456 #12345678";
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0].type == CHTL::TokenType::HEX_LITERAL);
    REQUIRE(tokens[0].value == "#fff");
    REQUIRE(tokens[1].type == CHTL::TokenType::HEX_LITERAL);
    REQUIRE(tokens[1].value == "#123456");
    REQUIRE(tokens[2].type == CHTL::TokenType::HEX_LITERAL);
    REQUIRE(tokens[2].value == "#12345678");
}

TEST_CASE("Tokenize Parentheses", "[lexer]") {
    CHTL::CHTLLexer lexer;
    std::string input = "()";
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0].type == CHTL::TokenType::L_PAREN);
    REQUIRE(tokens[1].type == CHTL::TokenType::R_PAREN);
}

TEST_CASE("Tokenize Origin Keyword", "[lexer]") {
    CHTL::CHTLLexer lexer;
    std::string input = "Origin";
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == CHTL::TokenType::ORIGIN_KEYWORD);
    REQUIRE(tokens[0].value == "Origin");
}