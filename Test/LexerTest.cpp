#include "catch.hpp"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLLexer/Token.h"
#include <vector>

TEST_CASE("Lexer handles single-line comments", "[lexer]") {
    const std::string input = R"(
// This is a comment
div { }
)";
    CHTLLexer lexer(input);
    std::vector<Token> tokens = lexer.getAllTokens();

    REQUIRE(tokens.size() == 4);
    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[0].value == "div");
    REQUIRE(tokens[1].type == TokenType::LBrace);
    REQUIRE(tokens[2].type == TokenType::RBrace);
    REQUIRE(tokens[3].type == TokenType::EndOfFile);
}

TEST_CASE("Lexer handles string literals", "[lexer]") {
    const std::string input = R"(
div {
    text: "double-quoted string";
    id: 'single-quoted-string';
}
)";
    CHTLLexer lexer(input);
    std::vector<Token> tokens = lexer.getAllTokens();

    REQUIRE(tokens.size() == 12);
    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[0].value == "div");
    REQUIRE(tokens[1].type == TokenType::LBrace);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[2].value == "text");
    REQUIRE(tokens[3].type == TokenType::Colon);
    REQUIRE(tokens[4].type == TokenType::String);
    REQUIRE(tokens[4].value == "double-quoted string");
    REQUIRE(tokens[5].type == TokenType::Semicolon);
    REQUIRE(tokens[6].type == TokenType::Identifier);
    REQUIRE(tokens[6].value == "id");
    REQUIRE(tokens[7].type == TokenType::Colon);
    REQUIRE(tokens[8].type == TokenType::String);
    REQUIRE(tokens[8].value == "single-quoted-string");
    REQUIRE(tokens[9].type == TokenType::Semicolon);
    REQUIRE(tokens[10].type == TokenType::RBrace);
    REQUIRE(tokens[11].type == TokenType::EndOfFile);
}

TEST_CASE("Lexer handles generator comments", "[lexer]") {
    const std::string input = R"(
# This is a generator comment
p { }
)";
    CHTLLexer lexer(input);
    std::vector<Token> tokens = lexer.getAllTokens();

    REQUIRE(tokens.size() == 5);
    REQUIRE(tokens[0].type == TokenType::Comment);
    REQUIRE(tokens[0].value == "This is a generator comment");
    REQUIRE(tokens[1].type == TokenType::Identifier);
    REQUIRE(tokens[1].value == "p");
    REQUIRE(tokens[2].type == TokenType::LBrace);
    REQUIRE(tokens[3].type == TokenType::RBrace);
    REQUIRE(tokens[4].type == TokenType::EndOfFile);
}

TEST_CASE("Lexer handles multi-line comments", "[lexer]") {
    const std::string input = R"(
/* This is a
   multi-line comment */
span { }
)";
    CHTLLexer lexer(input);
    std::vector<Token> tokens = lexer.getAllTokens();

    REQUIRE(tokens.size() == 4);
    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[0].value == "span");
    REQUIRE(tokens[1].type == TokenType::LBrace);
    REQUIRE(tokens[2].type == TokenType::RBrace);
    REQUIRE(tokens[3].type == TokenType::EndOfFile);
}