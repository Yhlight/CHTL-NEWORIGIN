#include "gtest/gtest.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include "../../CHTL/CHTLNode/CommentNode.h"
#include "../../CHTL/CHTLNode/TextNode.h"
#include "../../CHTL/CHTLNode/ElementNode.h"
#include <sstream>

TEST(ParserTest, HandlesComments) {
    std::vector<Token> tokens = {
        {TokenType::SINGLE_LINE_COMMENT, "// single line"},
        {TokenType::MULTI_LINE_COMMENT, "/* multi line */"},
        {TokenType::GENERATOR_COMMENT, "# generator"}
    };

    CHTLParser parser(tokens);
    auto ast = parser.parse();

    ASSERT_EQ(ast.size(), 3);
    EXPECT_NE(dynamic_cast<CommentNode*>(ast[0].get()), nullptr);
    EXPECT_EQ(ast[0]->toString(), "CommentNode: // single line");
    EXPECT_NE(dynamic_cast<CommentNode*>(ast[1].get()), nullptr);
    EXPECT_EQ(ast[1]->toString(), "CommentNode: /* multi line */");
    EXPECT_NE(dynamic_cast<CommentNode*>(ast[2].get()), nullptr);
    EXPECT_EQ(ast[2]->toString(), "CommentNode: # generator");
}

TEST(ParserTest, HandlesTextNodes) {
    std::vector<Token> tokens = {
        {TokenType::TEXT_KEYWORD, "text"},
        {TokenType::LEFT_BRACE, "{"},
        {TokenType::STRING_LITERAL, "\"Hello World\""},
        {TokenType::RIGHT_BRACE, "}"}
    };

    CHTLParser parser(tokens);
    auto ast = parser.parse();

    ASSERT_EQ(ast.size(), 1);
    auto* textNode = dynamic_cast<TextNode*>(ast[0].get());
    EXPECT_NE(textNode, nullptr);
    EXPECT_EQ(textNode->toString(), "TextNode: \"Hello World\"");
}

TEST(ParserTest, HandlesUnquotedTextNodes) {
    std::vector<Token> tokens = {
        {TokenType::TEXT_KEYWORD, "text"},
        {TokenType::LEFT_BRACE, "{"},
        {TokenType::UNQUOTED_LITERAL, "Hello"},
        {TokenType::UNQUOTED_LITERAL, "Unquoted"},
        {TokenType::UNQUOTED_LITERAL, "World"},
        {TokenType::RIGHT_BRACE, "}"}
    };

    CHTLParser parser(tokens);
    auto ast = parser.parse();

    ASSERT_EQ(ast.size(), 1);
    auto* textNode = dynamic_cast<TextNode*>(ast[0].get());
    EXPECT_NE(textNode, nullptr);
    EXPECT_EQ(textNode->toString(), "TextNode: Hello Unquoted World");
}

TEST(ParserTest, HandlesNestedElements) {
    std::vector<Token> tokens = {
        {TokenType::UNQUOTED_LITERAL, "div"},
        {TokenType::LEFT_BRACE, "{"},
        {TokenType::TEXT_KEYWORD, "text"},
        {TokenType::LEFT_BRACE, "{"},
        {TokenType::STRING_LITERAL, "\"nested text\""},
        {TokenType::RIGHT_BRACE, "}"},
        {TokenType::RIGHT_BRACE, "}"}
    };

    CHTLParser parser(tokens);
    auto ast = parser.parse();

    ASSERT_EQ(ast.size(), 1);
    auto* root = dynamic_cast<ElementNode*>(ast[0].get());
    ASSERT_NE(root, nullptr);

    std::stringstream expected;
    expected << "ElementNode(div):\n"
             << "  TextNode: \"nested text\"\n";

    EXPECT_EQ(root->toString(), expected.str());
}