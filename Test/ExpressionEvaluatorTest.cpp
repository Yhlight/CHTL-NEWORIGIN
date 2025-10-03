#include "gtest/gtest.h"
#include "../src/Util/ExpressionEvaluator/ExpressionEvaluator.h"
#include "../src/CHTL/CHTLNode/ValueNode.h"
#include "../src/CHTL/CHTLNode/BinaryOpNode.h"
#include "../src/CHTL/CHTLLexer/Token.h"
#include <memory>

class ExpressionEvaluatorTest : public ::testing::Test {
protected:
    ExpressionEvaluator evaluator;
};

TEST_F(ExpressionEvaluatorTest, HandlesSimpleAddition) {
    auto left = std::make_unique<ValueNode>("10px");
    auto right = std::make_unique<ValueNode>("5px");
    auto expr = std::make_unique<BinaryOpNode>(TokenType::PLUS, std::move(left), std::move(right));

    EXPECT_EQ(evaluator.evaluate(expr.get()), "15px");
}

TEST_F(ExpressionEvaluatorTest, HandlesSimpleSubtraction) {
    auto left = std::make_unique<ValueNode>("20em");
    auto right = std::make_unique<ValueNode>("5em");
    auto expr = std::make_unique<BinaryOpNode>(TokenType::MINUS, std::move(left), std::move(right));

    EXPECT_EQ(evaluator.evaluate(expr.get()), "15em");
}

TEST_F(ExpressionEvaluatorTest, HandlesMultiplication) {
    auto left = std::make_unique<ValueNode>("10%");
    auto right = std::make_unique<ValueNode>("5");
    auto expr = std::make_unique<BinaryOpNode>(TokenType::STAR, std::move(left), std::move(right));

    EXPECT_EQ(evaluator.evaluate(expr.get()), "50%");
}

TEST_F(ExpressionEvaluatorTest, HandlesDivision) {
    auto left = std::make_unique<ValueNode>("100px");
    auto right = std::make_unique<ValueNode>("4");
    auto expr = std::make_unique<BinaryOpNode>(TokenType::SLASH, std::move(left), std::move(right));

    EXPECT_EQ(evaluator.evaluate(expr.get()), "25px");
}

TEST_F(ExpressionEvaluatorTest, HandlesComplexExpression) {
    // Represents "10px + 5px * 2"
    auto five_px = std::make_unique<ValueNode>("5px");
    auto two = std::make_unique<ValueNode>("2");
    auto mult_expr = std::make_unique<BinaryOpNode>(TokenType::STAR, std::move(five_px), std::move(two));

    auto ten_px = std::make_unique<ValueNode>("10px");
    auto add_expr = std::make_unique<BinaryOpNode>(TokenType::PLUS, std::move(ten_px), std::move(mult_expr));

    EXPECT_EQ(evaluator.evaluate(add_expr.get()), "20px");
}

TEST_F(ExpressionEvaluatorTest, ThrowsOnMismatchedUnitsAddition) {
    auto left = std::make_unique<ValueNode>("10px");
    auto right = std::make_unique<ValueNode>("5em");
    auto expr = std::make_unique<BinaryOpNode>(TokenType::PLUS, std::move(left), std::move(right));

    EXPECT_THROW(evaluator.evaluate(expr.get()), std::runtime_error);
}

TEST_F(ExpressionEvaluatorTest, ThrowsOnDivisionByZero) {
    auto left = std::make_unique<ValueNode>("100px");
    auto right = std::make_unique<ValueNode>("0");
    auto expr = std::make_unique<BinaryOpNode>(TokenType::SLASH, std::move(left), std::move(right));

    EXPECT_THROW(evaluator.evaluate(expr.get()), std::runtime_error);
}

TEST_F(ExpressionEvaluatorTest, ThrowsOnMultiplyTwoUnits) {
    auto left = std::make_unique<ValueNode>("10px");
    auto right = std::make_unique<ValueNode>("2em");
    auto expr = std::make_unique<BinaryOpNode>(TokenType::STAR, std::move(left), std::move(right));

    EXPECT_THROW(evaluator.evaluate(expr.get()), std::runtime_error);
}