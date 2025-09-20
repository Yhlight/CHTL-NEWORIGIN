#include <gtest/gtest.h>
#include "CHTLCommon.h"
#include "Token.h"
#include "../src/CHTL/CHTLLexer/CHTLLexer.h"

using namespace CHTL;

class LexerTest : public ::testing::Test {
protected:
    void SetUp() override {
        lexer = std::make_unique<CHTLLexer>("", "");
    }
    
    void TearDown() override {
        lexer.reset();
    }
    
    std::unique_ptr<CHTLLexer> lexer;
};

TEST_F(LexerTest, BasicTokenization) {
    lexer->setSource("div { text: \"Hello World\"; }");
    auto tokens = lexer->tokenize();
    
    ASSERT_GT(tokens.size(), 0);
    EXPECT_EQ(tokens[0]->getType(), TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[0]->getValue(), "div");
}

TEST_F(LexerTest, StringLiteral) {
    lexer->setSource("\"Hello World\"");
    auto tokens = lexer->tokenize();
    
    ASSERT_GT(tokens.size(), 0);
    EXPECT_EQ(tokens[0]->getType(), TokenType::STRING_LITERAL);
    EXPECT_EQ(tokens[0]->getValue(), "Hello World");
}

TEST_F(LexerTest, NumberLiteral) {
    lexer->setSource("123.45");
    auto tokens = lexer->tokenize();
    
    ASSERT_GT(tokens.size(), 0);
    EXPECT_EQ(tokens[0]->getType(), TokenType::NUMBER_LITERAL);
    EXPECT_EQ(tokens[0]->getValue(), "123.45");
}

TEST_F(LexerTest, Operators) {
    lexer->setSource("+ - * / % **");
    auto tokens = lexer->tokenize();
    
    ASSERT_GE(tokens.size(), 6);
    EXPECT_EQ(tokens[0]->getType(), TokenType::PLUS);
    EXPECT_EQ(tokens[1]->getType(), TokenType::MINUS);
    EXPECT_EQ(tokens[2]->getType(), TokenType::MULTIPLY);
    EXPECT_EQ(tokens[3]->getType(), TokenType::DIVIDE);
    EXPECT_EQ(tokens[4]->getType(), TokenType::MODULO);
    EXPECT_EQ(tokens[5]->getType(), TokenType::POWER);
}

TEST_F(LexerTest, Brackets) {
    lexer->setSource("{ } ( ) [ ]");
    auto tokens = lexer->tokenize();
    
    ASSERT_GE(tokens.size(), 6);
    EXPECT_EQ(tokens[0]->getType(), TokenType::LEFT_BRACE);
    EXPECT_EQ(tokens[1]->getType(), TokenType::RIGHT_BRACE);
    EXPECT_EQ(tokens[2]->getType(), TokenType::LEFT_PAREN);
    EXPECT_EQ(tokens[3]->getType(), TokenType::RIGHT_PAREN);
    EXPECT_EQ(tokens[4]->getType(), TokenType::LEFT_BRACKET);
    EXPECT_EQ(tokens[5]->getType(), TokenType::RIGHT_BRACKET);
}

TEST_F(LexerTest, Keywords) {
    lexer->setSource("text style script template custom");
    auto tokens = lexer->tokenize();
    
    ASSERT_GE(tokens.size(), 5);
    EXPECT_EQ(tokens[0]->getType(), TokenType::TEXT);
    EXPECT_EQ(tokens[1]->getType(), TokenType::STYLE);
    EXPECT_EQ(tokens[2]->getType(), TokenType::SCRIPT);
    EXPECT_EQ(tokens[3]->getType(), TokenType::TEMPLATE);
    EXPECT_EQ(tokens[4]->getType(), TokenType::CUSTOM);
}

TEST_F(LexerTest, Comments) {
    lexer->setSource("// This is a comment\n/* Multi-line comment */");
    auto tokens = lexer->tokenize();
    
    ASSERT_GE(tokens.size(), 2);
    EXPECT_EQ(tokens[0]->getType(), TokenType::COMMENT);
    EXPECT_EQ(tokens[1]->getType(), TokenType::COMMENT);
}

TEST_F(LexerTest, GeneratorComments) {
    lexer->setSource("# This is a generator comment");
    auto tokens = lexer->tokenize();
    
    ASSERT_GT(tokens.size(), 0);
    EXPECT_EQ(tokens[0]->getType(), TokenType::GENERATOR_COMMENT);
    EXPECT_EQ(tokens[0]->getValue(), "This is a generator comment");
}

TEST_F(LexerTest, SpecialSymbols) {
    lexer->setSource("@ # $ &");
    auto tokens = lexer->tokenize();
    
    ASSERT_GE(tokens.size(), 4);
    EXPECT_EQ(tokens[0]->getType(), TokenType::AT_SYMBOL);
    EXPECT_EQ(tokens[1]->getType(), TokenType::HASH_SYMBOL);
    EXPECT_EQ(tokens[2]->getType(), TokenType::DOLLAR_SYMBOL);
    EXPECT_EQ(tokens[3]->getType(), TokenType::AMPERSAND_SYMBOL);
}

TEST_F(LexerTest, ComplexExpression) {
    lexer->setSource("div { width: 100px + 50px; height: 200px; }");
    auto tokens = lexer->tokenize();
    
    ASSERT_GT(tokens.size(), 0);
    
    // Check for key tokens
    bool foundDiv = false;
    bool foundWidth = false;
    bool foundPlus = false;
    
    for (const auto& token : tokens) {
        if (token->getValue() == "div") foundDiv = true;
        if (token->getValue() == "width") foundWidth = true;
        if (token->getType() == TokenType::PLUS) foundPlus = true;
    }
    
    EXPECT_TRUE(foundDiv);
    EXPECT_TRUE(foundWidth);
    EXPECT_TRUE(foundPlus);
}

TEST_F(LexerTest, EmptyInput) {
    lexer->setSource("");
    auto tokens = lexer->tokenize();
    
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0]->getType(), TokenType::EOF_TOKEN);
}

TEST_F(LexerTest, WhitespaceHandling) {
    lexer->setSource("   div   {   text   :   \"Hello\"   ;   }   ");
    auto tokens = lexer->tokenize();
    
    // Should have meaningful tokens without excessive whitespace
    ASSERT_GT(tokens.size(), 0);
    
    // Check that we have the expected tokens
    bool foundDiv = false;
    bool foundText = false;
    bool foundHello = false;
    
    for (const auto& token : tokens) {
        if (token->getValue() == "div") foundDiv = true;
        if (token->getValue() == "text") foundText = true;
        if (token->getValue() == "Hello") foundHello = true;
    }
    
    EXPECT_TRUE(foundDiv);
    EXPECT_TRUE(foundText);
    EXPECT_TRUE(foundHello);
}