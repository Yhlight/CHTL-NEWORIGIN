#include <gtest/gtest.h>
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include <vector>

TEST(LexerTest, TokenizesIdentifiersAndBraces) {
    std::string source = "html { body {} }";
    CHTL::CHTLLexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.getAllTokens();

    // The correct token count is 7: html, {, body, {, }, }, EOF
    ASSERT_EQ(tokens.size(), 7);
    EXPECT_EQ(tokens[0].type, CHTL::TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[0].value, "html");
    EXPECT_EQ(tokens[1].type, CHTL::TokenType::LEFT_BRACE);
    EXPECT_EQ(tokens[2].type, CHTL::TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[2].value, "body");
    EXPECT_EQ(tokens[3].type, CHTL::TokenType::LEFT_BRACE);
    EXPECT_EQ(tokens[4].type, CHTL::TokenType::RIGHT_BRACE);
    EXPECT_EQ(tokens[5].type, CHTL::TokenType::RIGHT_BRACE);
    EXPECT_EQ(tokens[6].type, CHTL::TokenType::END_OF_FILE);
}

TEST(LexerTest, TokenizesAttributes) {
    std::string source = "id: \"main\";";
    CHTL::CHTLLexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.getAllTokens();

    ASSERT_EQ(tokens.size(), 5);
    EXPECT_EQ(tokens[0].type, CHTL::TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[0].value, "id");
    EXPECT_EQ(tokens[1].type, CHTL::TokenType::COLON);
    EXPECT_EQ(tokens[2].type, CHTL::TokenType::STRING_LITERAL);
    EXPECT_EQ(tokens[2].value, "main");
    EXPECT_EQ(tokens[3].type, CHTL::TokenType::SEMICOLON);
    EXPECT_EQ(tokens[4].type, CHTL::TokenType::END_OF_FILE);
}

TEST(LexerTest, SkipsComments) {
    std::string source = "// this is a comment\n div {}";
    CHTL::CHTLLexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.getAllTokens();

    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[0].type, CHTL::TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[0].value, "div");
}

TEST(LexerTest, TokenizesGeneratorComment) {
    std::string source = "# a special comment";
    CHTL::CHTLLexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.getAllTokens();

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, CHTL::TokenType::GENERATOR_COMMENT);
    EXPECT_EQ(tokens[0].value, "a special comment");
}

TEST(LexerTest, HandlesKeywords) {
    std::string source = "text style script";
    CHTL::CHTLLexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.getAllTokens();

    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[0].type, CHTL::TokenType::KEYWORD_TEXT);
    EXPECT_EQ(tokens[1].type, CHTL::TokenType::KEYWORD_STYLE);
    EXPECT_EQ(tokens[2].type, CHTL::TokenType::KEYWORD_SCRIPT);
}
