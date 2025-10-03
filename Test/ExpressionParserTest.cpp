#include "gtest/gtest.h"
#include "../src/CHTL/CHTLLexer/CHTLLexer.h"
#include "../src/CHTL/CHTLParser/CHTLParser.h"
#include "../src/CHTL/ExpressionParser/ExpressionParser.h"
#include "../src/CHTL/CHTLNode/ValueNode.h"
#include "../src/CHTL/CHTLNode/BinaryOpNode.h"
#include <memory>

class ExpressionParserTest : public ::testing::Test {
protected:
    std::unique_ptr<ExpressionNode> parseExpression(const std::string& input) {
        // The CHTLParser takes the raw string and creates its own lexer.
        // The ExpressionParser then uses the CHTLParser to get tokens.
        CHTLParser parser(input);
        ExpressionParser exprParser(parser);
        return exprParser.parse();
    }
};

TEST_F(ExpressionParserTest, ParsesSingleNumber) {
    auto node = parseExpression("100px");
    ASSERT_NE(node, nullptr);
    ValueNode* valueNode = dynamic_cast<ValueNode*>(node.get());
    ASSERT_NE(valueNode, nullptr);
    EXPECT_EQ(valueNode->getValue(), "100px");
}

TEST_F(ExpressionParserTest, ParsesSimpleAddition) {
    auto node = parseExpression("10px + 5px");
    ASSERT_NE(node, nullptr);
    BinaryOpNode* opNode = dynamic_cast<BinaryOpNode*>(node.get());
    ASSERT_NE(opNode, nullptr);
    EXPECT_EQ(opNode->getOp(), TokenType::PLUS);

    ValueNode* left = dynamic_cast<ValueNode*>(opNode->getLeft());
    ASSERT_NE(left, nullptr);
    EXPECT_EQ(left->getValue(), "10px");

    ValueNode* right = dynamic_cast<ValueNode*>(opNode->getRight());
    ASSERT_NE(right, nullptr);
    EXPECT_EQ(right->getValue(), "5px");
}

TEST_F(ExpressionParserTest, HandlesOperatorPrecedence) {
    auto node = parseExpression("10px + 5px * 2");
    ASSERT_NE(node, nullptr);

    // Root should be '+'
    BinaryOpNode* plusNode = dynamic_cast<BinaryOpNode*>(node.get());
    ASSERT_NE(plusNode, nullptr);
    EXPECT_EQ(plusNode->getOp(), TokenType::PLUS);

    // Left of '+' should be '10px'
    ValueNode* leftVal = dynamic_cast<ValueNode*>(plusNode->getLeft());
    ASSERT_NE(leftVal, nullptr);
    EXPECT_EQ(leftVal->getValue(), "10px");

    // Right of '+' should be a '*' expression
    BinaryOpNode* starNode = dynamic_cast<BinaryOpNode*>(plusNode->getRight());
    ASSERT_NE(starNode, nullptr);
    EXPECT_EQ(starNode->getOp(), TokenType::STAR);

    // Left of '*' should be '5px'
    ValueNode* innerLeft = dynamic_cast<ValueNode*>(starNode->getLeft());
    ASSERT_NE(innerLeft, nullptr);
    EXPECT_EQ(innerLeft->getValue(), "5px");

    // Right of '*' should be '2'
    ValueNode* innerRight = dynamic_cast<ValueNode*>(starNode->getRight());
    ASSERT_NE(innerRight, nullptr);
    EXPECT_EQ(innerRight->getValue(), "2");
}

TEST_F(ExpressionParserTest, HandlesParentheses) {
    auto node = parseExpression("(10px + 5px) * 2");
    ASSERT_NE(node, nullptr);

    // Root should be '*'
    BinaryOpNode* starNode = dynamic_cast<BinaryOpNode*>(node.get());
    ASSERT_NE(starNode, nullptr);
    EXPECT_EQ(starNode->getOp(), TokenType::STAR);

    // Right of '*' should be '2'
    ValueNode* rightVal = dynamic_cast<ValueNode*>(starNode->getRight());
    ASSERT_NE(rightVal, nullptr);
    EXPECT_EQ(rightVal->getValue(), "2");

    // Left of '*' should be a '+' expression
    BinaryOpNode* plusNode = dynamic_cast<BinaryOpNode*>(starNode->getLeft());
    ASSERT_NE(plusNode, nullptr);
    EXPECT_EQ(plusNode->getOp(), TokenType::PLUS);

    // Left of '+' should be '10px'
    ValueNode* innerLeft = dynamic_cast<ValueNode*>(plusNode->getLeft());
    ASSERT_NE(innerLeft, nullptr);
    EXPECT_EQ(innerLeft->getValue(), "10px");

    // Right of '+' should be '5px'
    ValueNode* innerRight = dynamic_cast<ValueNode*>(plusNode->getRight());
    ASSERT_NE(innerRight, nullptr);
    EXPECT_EQ(innerRight->getValue(), "5px");
}