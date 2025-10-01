#include "catch.hpp"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLEvaluator/ExpressionEvaluator.h"
#include "CHTLNode/ExpressionNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/StylePropertyNode.h"
#include <memory>

// Helper to evaluate an expression within a given style context
CHTL::EvaluatedValue evaluate_with_context(const std::string& style_block_str, const std::string& prop_to_eval) {
    std::string input = "div { style { " + style_block_str + " } }";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    auto root = parser.parse();
    auto element = static_cast<CHTL::ElementNode*>(root.get());
    const auto* style = element->getStyle();

    const CHTL::ExpressionNode* expr_to_eval = nullptr;
    for (const auto& prop : style->getProperties()) {
        if (prop->getKey() == prop_to_eval) {
            expr_to_eval = prop->getValue();
            break;
        }
    }
    REQUIRE(expr_to_eval != nullptr);

    CHTL::ExpressionEvaluator evaluator;
    return evaluator.evaluate(expr_to_eval, style);
}


TEST_CASE("ExpressionEvaluator with conditional logic", "[evaluator]") {
    SECTION("Simple ternary operator") {
        auto result = evaluate_with_context("width: 100px; background: width > 50px ? 'red' : 'blue';", "background");
        REQUIRE(result.type == CHTL::EvaluatedValue::Type::STRING);
        REQUIRE(result.string_value == "red");

        auto result2 = evaluate_with_context("width: 40px; background: width > 50px ? 'red' : 'blue';", "background");
        REQUIRE(result2.type == CHTL::EvaluatedValue::Type::STRING);
        REQUIRE(result2.string_value == "blue");
    }

    SECTION("Logical AND") {
        auto result = evaluate_with_context("width: 60px; height: 120px; background: width > 50px && height > 100px ? 'green' : 'black';", "background");
        REQUIRE(result.string_value == "green");
    }

    SECTION("Logical OR") {
        auto result = evaluate_with_context("width: 40px; height: 120px; background: width > 50px || height > 100px ? 'yellow' : 'black';", "background");
        REQUIRE(result.string_value == "yellow");
    }

    SECTION("Equality with strings") {
        auto result = evaluate_with_context("display: 'block'; background: display == 'block' ? 'red' : 'blue';", "background");
        REQUIRE(result.string_value == "red");
    }
}