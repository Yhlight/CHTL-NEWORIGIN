#include "../third-party/catch.hpp"
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLLexer/Token.h"

#include <vector>
#include <string>
#include <utility>

TEST_CASE("Lexer tokenizes simple input", "[lexer]") {
    std::string input = R"(
div {
    id: "main";
    text { "hello" }
}
)";
    CHTL::CHTLLexer lexer(input);
    std::vector<std::pair<CHTL::TokenType, std::string>> expectedTokens = {
        {CHTL::TokenType::TOKEN_IDENTIFIER, "div"},
        {CHTL::TokenType::TOKEN_LBRACE, "{"},
        {CHTL::TokenType::TOKEN_IDENTIFIER, "id"},
        {CHTL::TokenType::TOKEN_COLON, ":"},
        {CHTL::TokenType::TOKEN_STRING_LITERAL, "main"},
        {CHTL::TokenType::TOKEN_SEMICOLON, ";"},
        {CHTL::TokenType::TOKEN_TEXT, "text"},
        {CHTL::TokenType::TOKEN_LBRACE, "{"},
        {CHTL::TokenType::TOKEN_STRING_LITERAL, "hello"},
        {CHTL::TokenType::TOKEN_RBRACE, "}"},
        {CHTL::TokenType::TOKEN_RBRACE, "}"}
    };

    for (const auto& expected : expectedTokens) {
        CHTL::Token token = lexer.getNextToken();
        REQUIRE(token.type == expected.first);
        REQUIRE(token.lexeme == expected.second);
    }
    REQUIRE(lexer.getNextToken().type == CHTL::TokenType::TOKEN_EOF);
}

TEST_CASE("Lexer handles comments", "[lexer]") {
    std::string input = R"(
// this is a comment
div { # another comment
    /* multi-line
       comment */
    id: "main"; // another comment
}
)";
    CHTL::CHTLLexer lexer(input);
    std::vector<CHTL::Token> tokens;
    CHTL::Token token = lexer.getNextToken();
    while (token.type != CHTL::TokenType::TOKEN_EOF) {
        // We are interested in non-comment tokens
        if (token.type != CHTL::TokenType::TOKEN_SINGLE_LINE_COMMENT &&
            token.type != CHTL::TokenType::TOKEN_MULTI_LINE_COMMENT &&
            token.type != CHTL::TokenType::TOKEN_GENERATOR_COMMENT) {
            tokens.push_back(token);
        }
        token = lexer.getNextToken();
    }

    std::vector<std::pair<CHTL::TokenType, std::string>> expectedTokens = {
        {CHTL::TokenType::TOKEN_IDENTIFIER, "div"},
        {CHTL::TokenType::TOKEN_LBRACE, "{"},
        {CHTL::TokenType::TOKEN_IDENTIFIER, "id"},
        {CHTL::TokenType::TOKEN_COLON, ":"},
        {CHTL::TokenType::TOKEN_STRING_LITERAL, "main"},
        {CHTL::TokenType::TOKEN_SEMICOLON, ";"},
        {CHTL::TokenType::TOKEN_RBRACE, "}"}
    };

    REQUIRE(tokens.size() == expectedTokens.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        REQUIRE(tokens[i].type == expectedTokens[i].first);
        REQUIRE(tokens[i].lexeme == expectedTokens[i].second);
    }
}