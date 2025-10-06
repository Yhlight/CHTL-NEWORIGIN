#include "../third-party/catch.hpp"
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLParser/ExpressionParser.h"
#include "../CHTL/CHTLEvaluator/ExpressionEvaluator.h"
#include "../CHTL/CHTLNode/ElementNode.h"
#include <memory>
#include <vector>
#include <string>
#include <any>

std::any evaluate_expression(const std::string& input, std::shared_ptr<CHTL::ElementNode> context = nullptr) {
    CHTL::CHTLLexer lexer(input, nullptr);
    std::vector<CHTL::Token> tokens;
    CHTL::Token token = lexer.getNextToken();
    while (token.type != CHTL::TokenType::TOKEN_EOF) {
        tokens.push_back(token);
        token = lexer.getNextToken();
    }

    CHTL::ExpressionParser parser(tokens);
    auto expression_tree = parser.parse();
    REQUIRE(expression_tree != nullptr);

    CHTL::ExpressionEvaluator evaluator(context);
    return evaluator.evaluate(expression_tree);
}

TEST_CASE("Expression parser handles simple numeric literal", "[expression]") {
    auto result = evaluate_expression("123");
    REQUIRE(std::any_cast<double>(result) == 123.0);
}

TEST_CASE("Expression parser handles addition and subtraction", "[expression]") {
    auto result = evaluate_expression("10 + 5 - 3");
    REQUIRE(std::any_cast<double>(result) == 12.0);
}

TEST_CASE("Expression parser handles multiplication and division", "[expression]") {
    auto result = evaluate_expression("10 * 5 / 2");
    REQUIRE(std::any_cast<double>(result) == 25.0);
}

TEST_CASE("Expression parser handles operator precedence", "[expression]") {
    auto result = evaluate_expression("10 + 5 * 2");
    REQUIRE(std::any_cast<double>(result) == 20.0);
}

TEST_CASE("Expression parser handles grouping with parentheses", "[expression]") {
    auto result = evaluate_expression("(10 + 5) * 2");
    REQUIRE(std::any_cast<double>(result) == 30.0);
}

TEST_CASE("Expression parser handles unary negation", "[expression]") {
    auto result = evaluate_expression("-10 + 5");
    REQUIRE(std::any_cast<double>(result) == -5.0);
}

TEST_CASE("Expression parser handles logical AND", "[expression]") {
    auto result_true = evaluate_expression("1 && 1");
    REQUIRE(std::any_cast<bool>(result_true) == true);

    auto result_false = evaluate_expression("1 && 0");
    REQUIRE(std::any_cast<bool>(result_false) == false);
}

TEST_CASE("Expression parser handles logical OR", "[expression]") {
    auto result_true = evaluate_expression("1 || 0");
    REQUIRE(std::any_cast<bool>(result_true) == true);

    auto result_false = evaluate_expression("0 || 0");
    REQUIRE(std::any_cast<bool>(result_false) == false);
}

TEST_CASE("Expression parser handles variable resolution", "[expression]") {
    auto context = std::make_shared<CHTL::ElementNode>("div");
    context->setAttribute("width", "150px");
    auto result = evaluate_expression("width > 100", context);
    REQUIRE(std::any_cast<bool>(result) == true);
}