#include <gtest/gtest.h>
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLLexer/Token.h"

// Helper function to check a token's properties
void checkToken(const Token& token, TokenType expectedType, const std::string& expectedValue) {
    EXPECT_EQ(token.type, expectedType);
    EXPECT_EQ(token.value, expectedValue);
}

TEST(LexerTest, TokenizesSimpleIdentifiers) {
    std::string input = "div";
    CHTLLexer lexer(input);
    Token token = lexer.getNextToken();
    checkToken(token, TokenType::IDENTIFIER, "div");
}

TEST(LexerTest, TokenizesKeywords) {
    std::string input = "text style script";
    CHTLLexer lexer(input);

    checkToken(lexer.getNextToken(), TokenType::KEYWORD_TEXT, "text");
    checkToken(lexer.getNextToken(), TokenType::KEYWORD_STYLE, "style");
    checkToken(lexer.getNextToken(), TokenType::KEYWORD_SCRIPT, "script");
}

TEST(LexerTest, TokenizesPunctuation) {
    std::string input = "{}:;=";
    CHTLLexer lexer(input);

    checkToken(lexer.getNextToken(), TokenType::LEFT_BRACE, "{");
    checkToken(lexer.getNextToken(), TokenType::RIGHT_BRACE, "}");
    checkToken(lexer.getNextToken(), TokenType::COLON, ":");
    checkToken(lexer.getNextToken(), TokenType::SEMICOLON, ";");
    checkToken(lexer.getNextToken(), TokenType::EQUALS, "=");
}

TEST(LexerTest, TokenizesStringLiterals) {
    std::string input = R"("hello" 'world')";
    CHTLLexer lexer(input);

    checkToken(lexer.getNextToken(), TokenType::STRING_LITERAL, "hello");
    checkToken(lexer.getNextToken(), TokenType::STRING_LITERAL, "world");
}

TEST(LexerTest, TokenizesUnquotedLiterals) {
    std::string input = "red 100px";
    CHTLLexer lexer(input);

    checkToken(lexer.getNextToken(), TokenType::IDENTIFIER, "red");
    checkToken(lexer.getNextToken(), TokenType::NUMBER, "100px");
}

TEST(LexerTest, TokenizesComments) {
    std::string input = R"(
        // single line
        /* multi
           line */
        # generator
    )";
    CHTLLexer lexer(input);

    checkToken(lexer.getNextToken(), TokenType::SINGLE_LINE_COMMENT, " single line");
    checkToken(lexer.getNextToken(), TokenType::MULTI_LINE_COMMENT, " multi\n           line ");
    checkToken(lexer.getNextToken(), TokenType::GENERATOR_COMMENT, " generator");
}

TEST(LexerTest, TokenizesComplexSnippet) {
    std::string input = R"(
        div {
            id: "main";
        }
    )";
    CHTLLexer lexer(input);

    checkToken(lexer.getNextToken(), TokenType::IDENTIFIER, "div");
    checkToken(lexer.getNextToken(), TokenType::LEFT_BRACE, "{");
    checkToken(lexer.getNextToken(), TokenType::IDENTIFIER, "id");
    checkToken(lexer.getNextToken(), TokenType::COLON, ":");
    checkToken(lexer.getNextToken(), TokenType::STRING_LITERAL, "main");
    checkToken(lexer.getNextToken(), TokenType::SEMICOLON, ";");
    checkToken(lexer.getNextToken(), TokenType::RIGHT_BRACE, "}");
    checkToken(lexer.getNextToken(), TokenType::END_OF_FILE, "");
}