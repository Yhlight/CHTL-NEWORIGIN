#include "gtest/gtest.h"
#include "../../CHTL/CHTLLexer/CHTLLexer.h"

TEST(CommentLexerTest, HandlesAllCommentTypes) {
    CHTLLexer lexer;
    std::string input = "// This is a single-line comment.\n"
                        "/* This is a multi-line comment. */\n"
                        "# This is a generator comment.\n"
                        "#This is not a generator comment.";

    std::vector<Token> tokens = lexer.tokenize(input);

    ASSERT_EQ(tokens.size(), 4);

    EXPECT_EQ(tokens[0].type, TokenType::SINGLE_LINE_COMMENT);
    EXPECT_EQ(tokens[0].value, "// This is a single-line comment.");

    EXPECT_EQ(tokens[1].type, TokenType::MULTI_LINE_COMMENT);
    EXPECT_EQ(tokens[1].value, "/* This is a multi-line comment. */");

    EXPECT_EQ(tokens[2].type, TokenType::GENERATOR_COMMENT);
    EXPECT_EQ(tokens[2].value, "# This is a generator comment.");

    EXPECT_EQ(tokens[3].type, TokenType::UNKNOWN);
    EXPECT_EQ(tokens[3].value, "#This");
}