#include "catch.hpp"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLEvaluator/ExpressionEvaluator.h"
#include "CHTLNode/ExpressionNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/StylePropertyNode.h"
#include "CHTLUtil/DocumentTraverser.h"
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
    return evaluator.evaluate(expr_to_eval, root.get(), style);
}

// Helper to evaluate a property reference expression
CHTL::EvaluatedValue evaluate_reference_expression(const std::string& document_str, const std::string& target_selector, const std::string& prop_to_eval) {
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(document_str);
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> root = parser.parse();

    const auto* target_element = CHTL::DocumentTraverser::find(root.get(), target_selector);
    REQUIRE(target_element != nullptr);

    const auto* style = target_element->getStyle();
    REQUIRE(style != nullptr);

    const CHTL::ExpressionNode* expr_to_eval = nullptr;
    for (const auto& prop : style->getProperties()) {
        if (prop->getKey() == prop_to_eval) {
            expr_to_eval = prop->getValue();
            break;
        }
    }
    REQUIRE(expr_to_eval != nullptr);

    CHTL::ExpressionEvaluator evaluator;
    return evaluator.evaluate(expr_to_eval, root.get(), style);
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

TEST_CASE("ExpressionEvaluator with property references", "[evaluator]") {
    std::string input = R"(
        body {
            div {
                class: "box";
                style { width: 100px; }
            }
            span {
                style {
                    width: .box.width + 50px;
                    color: .box.width > 50px ? 'green' : 'black';
                }
            }
        }
    )";

    SECTION("Simple reference with arithmetic") {
        auto result = evaluate_reference_expression(input, "span", "width");
        REQUIRE(result.type == CHTL::EvaluatedValue::Type::NUMBER);
        REQUIRE(result.number_value == 150.0);
        REQUIRE(result.unit == "px");
    }

    SECTION("Reference in conditional expression") {
        auto result = evaluate_reference_expression(input, "span", "color");
        REQUIRE(result.type == CHTL::EvaluatedValue::Type::STRING);
        REQUIRE(result.string_value == "green");
    }
}