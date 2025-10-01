#include "catch.hpp"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLEvaluator/ExpressionEvaluator.h"
#include "CHTLNode/ExpressionNode.h"

// Helper function to parse an expression string and return the AST
std::unique_ptr<CHTL::ExpressionNode> parse_expression_from_string(const std::string& expression_str) {
    std::string input = "div { style { width: " + expression_str + "; } }";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    auto root = parser.parse();
    auto element = static_cast<CHTL::ElementNode*>(root.get());
    auto style = element->getStyle();
    auto prop = style->getProperties()[0].get();

    // The parser returns a unique_ptr, but the evaluator takes a raw pointer.
    // We need to release ownership from the parser's result to avoid double-free.
    // This is a bit of a hack for testing, but it works.
    // In a real scenario, the generator would own the AST and pass raw pointers.
    auto* raw_expr = const_cast<CHTL::StylePropertyNode*>(prop)->releaseValue(); // Release ownership

    return std::unique_ptr<CHTL::ExpressionNode>(raw_expr);
}


TEST_CASE("ExpressionEvaluator evaluates literals", "[evaluator]") {
    CHTL::ExpressionEvaluator evaluator;
    auto expr = parse_expression_from_string("100px");
    CHTL::EvaluatedValue result = evaluator.evaluate(expr.get());
    REQUIRE(result.number_value == 100.0);
    REQUIRE(result.unit == "px");
}

TEST_CASE("ExpressionEvaluator handles addition", "[evaluator]") {
    CHTL::ExpressionEvaluator evaluator;
    SECTION("Compatible units") {
        auto expr = parse_expression_from_string("10px + 5px");
        CHTL::EvaluatedValue result = evaluator.evaluate(expr.get());
        REQUIRE(result.number_value == 15.0);
        REQUIRE(result.unit == "px");
    }
    SECTION("With unit-less number") {
        auto expr = parse_expression_from_string("10% + 5");
        CHTL::EvaluatedValue result = evaluator.evaluate(expr.get());
        REQUIRE(result.number_value == 15.0);
        REQUIRE(result.unit == "%");
    }
    SECTION("Incompatible units") {
        auto expr = parse_expression_from_string("10px + 5em");
        REQUIRE_THROWS(evaluator.evaluate(expr.get()));
    }
}

TEST_CASE("ExpressionEvaluator handles subtraction", "[evaluator]") {
    CHTL::ExpressionEvaluator evaluator;
    SECTION("Compatible units") {
        auto expr = parse_expression_from_string("10px - 5px");
        CHTL::EvaluatedValue result = evaluator.evaluate(expr.get());
        REQUIRE(result.number_value == 5.0);
        REQUIRE(result.unit == "px");
    }
    SECTION("With unit-less number") {
        auto expr = parse_expression_from_string("10% - 5");
        CHTL::EvaluatedValue result = evaluator.evaluate(expr.get());
        REQUIRE(result.number_value == 5.0);
        REQUIRE(result.unit == "%");
    }
    SECTION("Incompatible units") {
        auto expr = parse_expression_from_string("10px - 5em");
        REQUIRE_THROWS(evaluator.evaluate(expr.get()));
    }
}

TEST_CASE("ExpressionEvaluator handles multiplication", "[evaluator]") {
    CHTL::ExpressionEvaluator evaluator;
    SECTION("With unit-less number") {
        auto expr = parse_expression_from_string("10px * 2");
        CHTL::EvaluatedValue result = evaluator.evaluate(expr.get());
        REQUIRE(result.number_value == 20.0);
        REQUIRE(result.unit == "px");
    }
    SECTION("Multiplying two units") {
        auto expr = parse_expression_from_string("10px * 2px");
        REQUIRE_THROWS(evaluator.evaluate(expr.get()));
    }
}

TEST_CASE("ExpressionEvaluator handles division", "[evaluator]") {
    CHTL::ExpressionEvaluator evaluator;
    SECTION("By unit-less number") {
        auto expr = parse_expression_from_string("10px / 2");
        CHTL::EvaluatedValue result = evaluator.evaluate(expr.get());
        REQUIRE(result.number_value == 5.0);
        REQUIRE(result.unit == "px");
    }
    SECTION("Dividing two same units") {
        auto expr = parse_expression_from_string("10px / 2px");
        CHTL::EvaluatedValue result = evaluator.evaluate(expr.get());
        REQUIRE(result.number_value == 5.0);
        REQUIRE(result.unit == "");
    }
    SECTION("Dividing by a unit") {
        auto expr = parse_expression_from_string("10 / 2px");
        REQUIRE_THROWS(evaluator.evaluate(expr.get()));
    }
}

TEST_CASE("ExpressionEvaluator handles modulo and power", "[evaluator]") {
    CHTL::ExpressionEvaluator evaluator;
    SECTION("Modulo") {
        auto expr = parse_expression_from_string("10 % 3");
        CHTL::EvaluatedValue result = evaluator.evaluate(expr.get());
        REQUIRE(result.number_value == 1.0);
        REQUIRE(result.unit == "");
    }
     SECTION("Power") {
        auto expr = parse_expression_from_string("2 ** 3");
        CHTL::EvaluatedValue result = evaluator.evaluate(expr.get());
        REQUIRE(result.number_value == 8.0);
        REQUIRE(result.unit == "");
    }
    SECTION("With units") {
        auto expr_mod = parse_expression_from_string("10px % 3");
        REQUIRE_THROWS(evaluator.evaluate(expr_mod.get()));

        auto expr_pow = parse_expression_from_string("2px ** 3");
        REQUIRE_THROWS(evaluator.evaluate(expr_pow.get()));
    }
}