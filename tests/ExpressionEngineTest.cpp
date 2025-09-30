#include "../catch.hpp"
#include "../src/CHTL/ExpressionParser/ExpressionLexer.h"
#include "../src/CHTL/ExpressionParser/ExpressionParser.h"
#include "../src/CHTL/ExpressionEvaluator/ExpressionEvaluator.h"
#include "../src/CHTL/CHTLParser/Parser.h" // Needed for the evaluator context
#include "../src/CHTL/CHTLLexer/Lexer.h"   // Needed for the parser context
#include "../src/CHTL/ExpressionNode/PropertyRefNode.h"
#include "../src/CHTL/CHTLNode/ElementNode.h"
#include "../src/CHTL/CHTLNode/DynamicStyleNode.h"
#include "../src/CHTL/CHTLManage/TemplateManager.h"
#include "../src/CHTL/CHTLManage/ConfigurationManager.h"

// Helper function to evaluate an expression string
std::string evaluateExpression(const std::string& expr_str) {
    // We need a dummy Parser context for the evaluator
    std::string dummy_source = "div {}"; // Provide a minimal valid source
    Lexer dummy_lexer(dummy_source);
    TemplateManager tm;
    ConfigurationManager cm;
    Parser dummy_parser(dummy_lexer, tm, cm);
    dummy_parser.parse(); // Initialize the parser's internal state

    ExpressionLexer lexer(expr_str);
    auto tokens = lexer.tokenize();
    ExpressionParser parser(tokens);
    auto ast = parser.parse();
    ExpressionEvaluator evaluator(dummy_parser);
    return evaluator.evaluate(ast.get());
}

// Helper function to approximate string-to-double conversion for comparison
bool approx_equal(const std::string& s1, const std::string& s2) {
    try {
        return std::abs(std::stod(s1) - std::stod(s2)) < 1e-9;
    } catch (...) {
        return s1 == s2;
    }
}

TEST_CASE("Expression Engine: Basic Arithmetic", "[expression]") {
    REQUIRE(approx_equal(evaluateExpression("10 + 5"), "15.000000"));
    REQUIRE(approx_equal(evaluateExpression("10px + 5px"), "15.000000px"));
    REQUIRE(approx_equal(evaluateExpression("20 - 7"), "13.000000"));
    REQUIRE(approx_equal(evaluateExpression("8 * 4"), "32.000000"));
    REQUIRE(approx_equal(evaluateExpression("100 / 25"), "4.000000"));
}

TEST_CASE("Expression Engine: Operator Precedence", "[expression]") {
    REQUIRE(approx_equal(evaluateExpression("10 + 5 * 2"), "20.000000"));
    REQUIRE(approx_equal(evaluateExpression("(10 + 5) * 2"), "30.000000"));
    REQUIRE(approx_equal(evaluateExpression("100 / 2 * 5"), "250.000000"));
}

TEST_CASE("Expression Engine: Conditional (Ternary) Operator", "[expression]") {
    REQUIRE(evaluateExpression("1 > 0 ? 'yes' : 'no'") == "yes");
    REQUIRE(evaluateExpression("1 < 0 ? 'yes' : 'no'") == "no");
    REQUIRE(approx_equal(evaluateExpression("10 == 10 ? 100 : 50"), "100.000000"));
    REQUIRE(approx_equal(evaluateExpression("10 != 10 ? 100 : 50"), "50.000000"));
}

TEST_CASE("Expression Engine: Logical Operators", "[expression]") {
    REQUIRE(approx_equal(evaluateExpression("1 > 0 && 10 > 5"), "1"));
    REQUIRE(approx_equal(evaluateExpression("1 > 0 && 1 > 5"), "0"));
    REQUIRE(approx_equal(evaluateExpression("1 > 0 || 1 > 5"), "1"));
    REQUIRE(approx_equal(evaluateExpression("1 < 0 || 1 > 5"), "0"));
}

TEST_CASE("Expression Engine: Complex Expressions", "[expression]") {
    REQUIRE(approx_equal(evaluateExpression("(10 + 5 * 2) > 15 ? 100 : 50"), "100.000000"));
    REQUIRE(approx_equal(evaluateExpression("10 > 5 && 20 < 30 ? 1 : 0"), "1"));
}

TEST_CASE("Expression Parser: Property Reference", "[expression_parser]") {
    // This test only verifies that the PARSER can create the correct AST node.
    // The evaluator for this is not yet implemented.
    std::string expr_str = "box.width";
    ExpressionLexer lexer(expr_str);
    auto tokens = lexer.tokenize();
    ExpressionParser parser(tokens);
    auto ast = parser.parse();

    REQUIRE(ast->getType() == ExpressionNodeType::PropertyRef);
    auto* refNode = static_cast<PropertyRefNode*>(ast.get());
    REQUIRE(refNode->selector == "box");
    REQUIRE(refNode->propertyName == "width");
}

TEST_CASE("Expression Engine: End-to-End Property Reference Evaluation", "[expression_evaluator]") {
    std::string source = R"(
        div {
            id: "box1";
            style {
                width: 100px;
                height: 200px;
            }
        }
        div {
            id: "box2";
            style {
                // Reference the width of box1
                width: #box1.width + 50px;
            }
        }
    )";

    // 1. Parse the full CHTL source
    Lexer lexer(source);
    TemplateManager tm;
    ConfigurationManager cm;
    Parser parser(lexer, tm, cm);
    parser.parse();
    REQUIRE(parser.ast.size() == 2);

    // 2. Find the dynamic style node in the second box
    auto* box2 = static_cast<ElementNode*>(parser.ast[1].get());
    REQUIRE(box2->inlineStyles.count("width") == 1);
    auto* styleValue = box2->inlineStyles.at("width").get();
    REQUIRE(styleValue->getType() == StyleValueType::Dynamic);
    auto* dynamicNode = static_cast<DynamicStyleNode*>(styleValue);

    // 3. Evaluate the expression with the parser's context
    ExpressionEvaluator evaluator(parser);
    std::string result = evaluator.evaluate(dynamicNode->expressionAst.get());

    // 4. Check the result (100px + 50px)
    REQUIRE(approx_equal(result, "150.000000px"));
}