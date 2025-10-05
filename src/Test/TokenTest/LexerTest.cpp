#include "gtest/gtest.h"
#include "../../CHTL/CHTLLexer/CHTLLexer.h"

TEST(LexerTest, HandlesAllCommentTypes) {
    CHTLLexer lexer;
    std::string input = "// This is a single-line comment.\n"
                        "/* This is a multi-line comment. */\n"
                        "# This is a generator comment.\n"
                        "#ThisIsNotAGeneratorComment";

    std::vector<Token> tokens = lexer.tokenize(input);

    ASSERT_EQ(tokens.size(), 4);

    EXPECT_EQ(tokens[0].type, TokenType::SINGLE_LINE_COMMENT);
    EXPECT_EQ(tokens[0].value, "// This is a single-line comment.");

    EXPECT_EQ(tokens[1].type, TokenType::MULTI_LINE_COMMENT);
    EXPECT_EQ(tokens[1].value, "/* This is a multi-line comment. */");

    EXPECT_EQ(tokens[2].type, TokenType::GENERATOR_COMMENT);
    EXPECT_EQ(tokens[2].value, "# This is a generator comment.");

    EXPECT_EQ(tokens[3].type, TokenType::UNQUOTED_LITERAL);
    EXPECT_EQ(tokens[3].value, "#ThisIsNotAGeneratorComment");
}

TEST(LexerTest, HandlesTextNodesAndLiterals) {
    CHTLLexer lexer;
    std::string input = "text { \"This is a string literal.\" }\n"
                        "text { 'This is another string literal.' }\n"
                        "text { ThisIsAnUnquotedLiteral }";

    std::vector<Token> tokens = lexer.tokenize(input);

    ASSERT_EQ(tokens.size(), 12);

    // First text block
    EXPECT_EQ(tokens[0].type, TokenType::TEXT_KEYWORD);
    EXPECT_EQ(tokens[0].value, "text");
    EXPECT_EQ(tokens[1].type, TokenType::LEFT_BRACE);
    EXPECT_EQ(tokens[1].value, "{");
    EXPECT_EQ(tokens[2].type, TokenType::STRING_LITERAL);
    EXPECT_EQ(tokens[2].value, "\"This is a string literal.\"");
    EXPECT_EQ(tokens[3].type, TokenType::RIGHT_BRACE);
    EXPECT_EQ(tokens[3].value, "}");

    // Second text block
    EXPECT_EQ(tokens[4].type, TokenType::TEXT_KEYWORD);
    EXPECT_EQ(tokens[4].value, "text");
    EXPECT_EQ(tokens[5].type, TokenType::LEFT_BRACE);
    EXPECT_EQ(tokens[5].value, "{");
    EXPECT_EQ(tokens[6].type, TokenType::STRING_LITERAL);
    EXPECT_EQ(tokens[6].value, "'This is another string literal.'");
    EXPECT_EQ(tokens[7].type, TokenType::RIGHT_BRACE);
    EXPECT_EQ(tokens[7].value, "}");

    // Third text block
    EXPECT_EQ(tokens[8].type, TokenType::TEXT_KEYWORD);
    EXPECT_EQ(tokens[8].value, "text");
    EXPECT_EQ(tokens[9].type, TokenType::LEFT_BRACE);
    EXPECT_EQ(tokens[9].value, "{");
    EXPECT_EQ(tokens[10].type, TokenType::UNQUOTED_LITERAL);
    EXPECT_EQ(tokens[10].value, "ThisIsAnUnquotedLiteral");
    EXPECT_EQ(tokens[11].type, TokenType::RIGHT_BRACE);
    EXPECT_EQ(tokens[11].value, "}");
}