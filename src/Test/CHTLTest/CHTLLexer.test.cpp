#include <gtest/gtest.h>
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include <vector>

using namespace CHTL;

TEST(CHTLLexerTest, TokenizesBasicElements) {
    std::string source = R"(
        // This is a comment
        html {
            body {
                text: "Hello, World!";
            }
        }
    )";

    CHTLLexer lexer(source);
    std::vector<Token> tokens;
    Token token = lexer.getNextToken();
    while (token.type != TokenType::END_OF_FILE) {
        tokens.push_back(token);
        token = lexer.getNextToken();
    }

    // FIX: The original test was missing the closing braces.
    ASSERT_EQ(tokens.size(), 10);
    EXPECT_EQ(tokens[0].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[0].lexeme, "html");

    EXPECT_EQ(tokens[1].type, TokenType::LEFT_BRACE);

    EXPECT_EQ(tokens[2].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[2].lexeme, "body");

    EXPECT_EQ(tokens[3].type, TokenType::LEFT_BRACE);

    EXPECT_EQ(tokens[4].type, TokenType::KEYWORD_TEXT);
    EXPECT_EQ(tokens[4].lexeme, "text");

    EXPECT_EQ(tokens[5].type, TokenType::COLON);

    EXPECT_EQ(tokens[6].type, TokenType::STRING);
    EXPECT_EQ(tokens[6].lexeme, "Hello, World!");

    EXPECT_EQ(tokens[7].type, TokenType::SEMICOLON);

    EXPECT_EQ(tokens[8].type, TokenType::RIGHT_BRACE);

    EXPECT_EQ(tokens[9].type, TokenType::RIGHT_BRACE);
}

TEST(CHTLLexerTest, TokenizesUnquotedLiterals) {
    std::string source = "div { id: my_id; }";
    CHTLLexer lexer(source);

    std::vector<Token> tokens;
    Token token = lexer.getNextToken();
    while (token.type != TokenType::END_OF_FILE) {
        tokens.push_back(token);
        token = lexer.getNextToken();
    }

    // FIX: The original test was missing the closing brace.
    ASSERT_EQ(tokens.size(), 7);
    EXPECT_EQ(tokens[0].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[0].lexeme, "div");
    EXPECT_EQ(tokens[1].type, TokenType::LEFT_BRACE);
    EXPECT_EQ(tokens[2].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[2].lexeme, "id");
    EXPECT_EQ(tokens[3].type, TokenType::COLON);
    EXPECT_EQ(tokens[4].type, TokenType::IDENTIFIER); // Treated as identifier for now
    EXPECT_EQ(tokens[4].lexeme, "my_id");
    EXPECT_EQ(tokens[5].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[6].type, TokenType::RIGHT_BRACE);
}
