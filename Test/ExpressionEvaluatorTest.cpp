#include "gtest/gtest.h"
#include "../src/Util/ExpressionEvaluator/ExpressionEvaluator.h"
#include "../src/CHTL/CHTLNode/ValueNode.h"
#include "../src/CHTL/CHTLNode/BinaryOpNode.h"
#include "../src/CHTL/CHTLNode/ConditionalNode.h"
#include "../src/CHTL/CHTLNode/ReferenceNode.h"
#include "../src/CHTL/CHTLNode/ElementNode.h"
#include "../src/CHTL/CHTLNode/StyleNode.h"
#include "../src/CHTL/CHTLNode/StylePropertyNode.h"
#include "../src/CHTL/CHTLNode/BaseNode.h"
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

    EXPECT_EQ(evaluator.evaluate(expr.get(), nullptr), "15px");
}

TEST_F(ExpressionEvaluatorTest, HandlesSimpleSubtraction) {
    auto left = std::make_unique<ValueNode>("20em");
    auto right = std::make_unique<ValueNode>("5em");
    auto expr = std::make_unique<BinaryOpNode>(TokenType::MINUS, std::move(left), std::move(right));

    EXPECT_EQ(evaluator.evaluate(expr.get(), nullptr), "15em");
}

TEST_F(ExpressionEvaluatorTest, HandlesMultiplication) {
    auto left = std::make_unique<ValueNode>("10%");
    auto right = std::make_unique<ValueNode>("5");
    auto expr = std::make_unique<BinaryOpNode>(TokenType::STAR, std::move(left), std::move(right));

    EXPECT_EQ(evaluator.evaluate(expr.get(), nullptr), "50%");
}

TEST_F(ExpressionEvaluatorTest, HandlesDivision) {
    auto left = std::make_unique<ValueNode>("100px");
    auto right = std::make_unique<ValueNode>("4");
    auto expr = std::make_unique<BinaryOpNode>(TokenType::SLASH, std::move(left), std::move(right));

    EXPECT_EQ(evaluator.evaluate(expr.get(), nullptr), "25px");
}

TEST_F(ExpressionEvaluatorTest, HandlesComplexExpression) {
    // Represents "10px + 5px * 2"
    auto five_px = std::make_unique<ValueNode>("5px");
    auto two = std::make_unique<ValueNode>("2");
    auto mult_expr = std::make_unique<BinaryOpNode>(TokenType::STAR, std::move(five_px), std::move(two));

    auto ten_px = std::make_unique<ValueNode>("10px");
    auto add_expr = std::make_unique<BinaryOpNode>(TokenType::PLUS, std::move(ten_px), std::move(mult_expr));

    EXPECT_EQ(evaluator.evaluate(add_expr.get(), nullptr), "20px");
}

TEST_F(ExpressionEvaluatorTest, ThrowsOnMismatchedUnitsAddition) {
    auto left = std::make_unique<ValueNode>("10px");
    auto right = std::make_unique<ValueNode>("5em");
    auto expr = std::make_unique<BinaryOpNode>(TokenType::PLUS, std::move(left), std::move(right));

    EXPECT_THROW(evaluator.evaluate(expr.get(), nullptr), std::runtime_error);
}

TEST_F(ExpressionEvaluatorTest, ThrowsOnDivisionByZero) {
    auto left = std::make_unique<ValueNode>("100px");
    auto right = std::make_unique<ValueNode>("0");
    auto expr = std::make_unique<BinaryOpNode>(TokenType::SLASH, std::move(left), std::move(right));

    EXPECT_THROW(evaluator.evaluate(expr.get(), nullptr), std::runtime_error);
}

TEST_F(ExpressionEvaluatorTest, ThrowsOnMultiplyTwoUnits) {
    auto left = std::make_unique<ValueNode>("10px");
    auto right = std::make_unique<ValueNode>("2em");
    auto expr = std::make_unique<BinaryOpNode>(TokenType::STAR, std::move(left), std::move(right));

    EXPECT_THROW(evaluator.evaluate(expr.get(), nullptr), std::runtime_error);
}

// --- Reference Resolution Tests ---

TEST_F(ExpressionEvaluatorTest, ResolvesSimplePropertyReference) {
    // Build mock AST: div { style { width: 100px; } }
    auto root = std::make_unique<BaseNode>();
    auto div = std::make_unique<ElementNode>("div");
    div->setAttribute("id", "box");
    auto style = std::make_unique<StyleNode>();
    auto widthProp = std::make_unique<StylePropertyNode>("width", std::make_unique<ValueNode>("100px"));
    style->addChild(std::move(widthProp));
    div->addChild(std::move(style));
    root->addChild(std::move(div));

    // Expression to evaluate: #box.width
    auto expr = std::make_unique<ReferenceNode>("#box.width");

    EXPECT_EQ(evaluator.evaluate(expr.get(), root.get()), "100px");
}

TEST_F(ExpressionEvaluatorTest, ResolvesReferenceInExpression) {
    // Build mock AST: div { style { width: 100px; } }
    auto root = std::make_unique<BaseNode>();
    auto div = std::make_unique<ElementNode>("div");
    div->setAttribute("id", "box");
    auto style = std::make_unique<StyleNode>();
    auto widthProp = std::make_unique<StylePropertyNode>("width", std::make_unique<ValueNode>("100px"));
    style->addChild(std::move(widthProp));
    div->addChild(std::move(style));
    root->addChild(std::move(div));

    // Expression to evaluate: #box.width + 50px
    auto ref = std::make_unique<ReferenceNode>("#box.width");
    auto val = std::make_unique<ValueNode>("50px");
    auto expr = std::make_unique<BinaryOpNode>(TokenType::PLUS, std::move(ref), std::move(val));

    EXPECT_EQ(evaluator.evaluate(expr.get(), root.get()), "150px");
}

TEST_F(ExpressionEvaluatorTest, ThrowsOnCircularReference) {
    // Build mock AST:
    // div1.width -> div2.width
    // div2.width -> div1.width
    auto root = std::make_unique<BaseNode>();

    auto div1 = std::make_unique<ElementNode>("div");
    div1->setAttribute("id", "div1");
    auto style1 = std::make_unique<StyleNode>();
    auto widthProp1 = std::make_unique<StylePropertyNode>("width", std::make_unique<ReferenceNode>("#div2.width"));
    style1->addChild(std::move(widthProp1));
    div1->addChild(std::move(style1));

    auto div2 = std::make_unique<ElementNode>("div");
    div2->setAttribute("id", "div2");
    auto style2 = std::make_unique<StyleNode>();
    auto widthProp2 = std::make_unique<StylePropertyNode>("width", std::make_unique<ReferenceNode>("#div1.width"));
    style2->addChild(std::move(widthProp2));
    div2->addChild(std::move(style2));

    root->addChild(std::move(div1));
    root->addChild(std::move(div2));

    // Expression to evaluate: #div1.width
    auto expr = std::make_unique<ReferenceNode>("#div1.width");

    EXPECT_THROW(evaluator.evaluate(expr.get(), root.get()), std::runtime_error);
}

// --- Conditional Logic Tests ---

TEST_F(ExpressionEvaluatorTest, HandlesSimpleTernary) {
    // 10 > 5 ? "yes" : "no"
    auto condition = std::make_unique<BinaryOpNode>(TokenType::GREATER,
        std::make_unique<ValueNode>("10"), std::make_unique<ValueNode>("5"));
    auto true_expr = std::make_unique<ValueNode>("yes");
    auto false_expr = std::make_unique<ValueNode>("no");

    auto ternary_expr = std::make_unique<ConditionalNode>(std::move(condition), std::move(true_expr), std::move(false_expr));

    EXPECT_EQ(evaluator.evaluate(ternary_expr.get(), nullptr), "yes");
}

TEST_F(ExpressionEvaluatorTest, HandlesComplexConditionWithLogicalAnd) {
    // (10 > 5 && 2 < 1) ? "no" : "yes"
    auto ten_greater_five = std::make_unique<BinaryOpNode>(TokenType::GREATER,
        std::make_unique<ValueNode>("10"), std::make_unique<ValueNode>("5"));
    auto two_less_one = std::make_unique<BinaryOpNode>(TokenType::LESS,
        std::make_unique<ValueNode>("2"), std::make_unique<ValueNode>("1"));

    auto condition = std::make_unique<BinaryOpNode>(TokenType::LOGICAL_AND, std::move(ten_greater_five), std::move(two_less_one));

    auto true_expr = std::make_unique<ValueNode>("no");
    auto false_expr = std::make_unique<ValueNode>("yes");

    auto ternary_expr = std::make_unique<ConditionalNode>(std::move(condition), std::move(true_expr), std::move(false_expr));

    EXPECT_EQ(evaluator.evaluate(ternary_expr.get(), nullptr), "yes");
}

TEST_F(ExpressionEvaluatorTest, HandlesTernaryWithExpressionResult) {
    // 1 > 0 ? 10px + 5px : 2px
    auto condition = std::make_unique<BinaryOpNode>(TokenType::GREATER,
        std::make_unique<ValueNode>("1"), std::make_unique<ValueNode>("0"));

    auto true_expr = std::make_unique<BinaryOpNode>(TokenType::PLUS,
        std::make_unique<ValueNode>("10px"), std::make_unique<ValueNode>("5px"));

    auto false_expr = std::make_unique<ValueNode>("2px");

    auto ternary_expr = std::make_unique<ConditionalNode>(std::move(condition), std::move(true_expr), std::move(false_expr));

    EXPECT_EQ(evaluator.evaluate(ternary_expr.get(), nullptr), "15px");
}