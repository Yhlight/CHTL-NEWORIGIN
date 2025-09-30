#include "../catch.hpp"
#include "../src/CHTL/CHTLLexer/Lexer.h"
#include "../src/CHTL/CHTLParser/Parser.h"
#include "../src/CHTL/CHTLNode/ElementNode.h"
#include "../src/CHTL/CHTLNode/StyleValue.h"
#include "../src/CHTL/CHTLNode/DynamicStyleNode.h"
#include "../src/CHTL/ExpressionNode/ConditionalNode.h"
#include "../src/CHTL/ExpressionNode/BinaryOpNode.h"
#include "../src/CHTL/ExpressionNode/PropertyRefNode.h"
#include "../src/CHTL/ExpressionNode/LiteralNode.h"
#include "../src/CHTL/CHTLNode/ResponsiveValueNode.h"
#include "../src/CHTL/CHTLNode/StaticStyleNode.h"

TEST_CASE("CHTL Parser for Dynamic Attribute Conditional Expressions", "[CHTLParser]") {
    std::string source = R"(
        div {
            style {
                width: {{box}}->width > 2 ? "100px" : "50px";
            }
        }
    )";

    Lexer lexer(source);
    Parser parser(lexer, "test.chtl");
    auto ast = parser.parse();

    REQUIRE(ast.size() == 1);
    auto* div_node = dynamic_cast<ElementNode*>(ast[0].get());
    REQUIRE(div_node != nullptr);

    REQUIRE(div_node->inlineStyles.count("width"));
    const auto& style_value = div_node->inlineStyles.at("width");
    REQUIRE(style_value != nullptr);

    auto* dynamic_node = dynamic_cast<DynamicStyleNode*>(style_value.get());
    REQUIRE(dynamic_node != nullptr);

    REQUIRE(dynamic_node->expressionAst != nullptr);
    auto* cond_node = dynamic_cast<ConditionalNode*>(dynamic_node->expressionAst.get());
    REQUIRE(cond_node != nullptr);

    // Check the condition part: {{box}}->width > 2
    auto* condition_expr = dynamic_cast<BinaryOpNode*>(cond_node->condition.get());
    REQUIRE(condition_expr != nullptr);
    REQUIRE(condition_expr->op == ">");

    auto* left_op = dynamic_cast<PropertyRefNode*>(condition_expr->left.get());
    REQUIRE(left_op != nullptr);
    REQUIRE(left_op->selector == "{{box}}");
    REQUIRE(left_op->propertyName == "width");

    auto* right_op = dynamic_cast<LiteralNode*>(condition_expr->right.get());
    REQUIRE(right_op != nullptr);
    REQUIRE(right_op->value == "2");

    // Check the true branch: "100px"
    auto* true_branch = dynamic_cast<LiteralNode*>(cond_node->trueExpression.get());
    REQUIRE(true_branch != nullptr);
    REQUIRE(true_branch->value == "\"100px\"");

    // Check the false branch: "50px"
    auto* false_branch = dynamic_cast<LiteralNode*>(cond_node->falseExpression.get());
    REQUIRE(false_branch != nullptr);
    REQUIRE(false_branch->value == "\"50px\"");
}

TEST_CASE("CHTL Parser for Reactive Values", "[CHTLParser]") {
    std::string source = R"(
        div {
            id: "my-div";
            class: $boxClass$;
            style {
                width: $boxWidth$px;
            }
        }
    )";

    Lexer lexer(source);
    Parser parser(lexer, "test.chtl");
    auto ast = parser.parse();

    REQUIRE(ast.size() == 1);
    auto* div_node = dynamic_cast<ElementNode*>(ast[0].get());
    REQUIRE(div_node != nullptr);

    // Test reactive attribute
    REQUIRE(div_node->attributes.count("class"));
    auto* class_node = dynamic_cast<ResponsiveValueNode*>(div_node->attributes.at("class").get());
    REQUIRE(class_node != nullptr);
    REQUIRE(class_node->getVariableName() == "boxClass");

    // Test reactive style
    REQUIRE(div_node->inlineStyles.count("width"));
    auto* width_node = dynamic_cast<ResponsiveValueNode*>(div_node->inlineStyles.at("width").get());
    REQUIRE(width_node != nullptr);
    REQUIRE(width_node->getVariableName() == "boxWidth");
    REQUIRE(width_node->getUnit() == "px");

    // Test SharedContext for bindings
    const auto& context = parser.sharedContext;
    REQUIRE(context.responsiveBindings.count("boxClass"));
    const auto& class_bindings = context.responsiveBindings.at("boxClass");
    REQUIRE(class_bindings.size() == 1);
    REQUIRE(class_bindings[0].elementId == "my-div");
    REQUIRE(class_bindings[0].property == "class");
    REQUIRE(class_bindings[0].unit == "");

    REQUIRE(context.responsiveBindings.count("boxWidth"));
    const auto& width_bindings = context.responsiveBindings.at("boxWidth");
    REQUIRE(width_bindings.size() == 1);
    REQUIRE(width_bindings[0].elementId == "my-div");
    REQUIRE(width_bindings[0].property == "style.width");
    REQUIRE(width_bindings[0].unit == "px");
}

TEST_CASE("CHTL Parser for Style Template", "[CHTLParser]") {
    std::string source = R"(
        [Template] @Style DefaultText {
            color: black;
            font-size: 16px;
        }

        div {
            style {
                @Style DefaultText;
                background-color: red;
            }
        }
    )";

    Lexer lexer(source);
    Parser parser(lexer, "test.chtl");
    auto ast = parser.parse();

    // The AST should contain the div element, not the template definition.
    REQUIRE(ast.size() == 1);
    auto* div_node = dynamic_cast<ElementNode*>(ast[0].get());
    REQUIRE(div_node != nullptr);

    // The div should have three styles: two from the template and one direct.
    REQUIRE(div_node->inlineStyles.size() == 3);

    // Check that the styles from the template were applied correctly.
    REQUIRE(div_node->inlineStyles.count("color"));
    auto* color_value = dynamic_cast<StaticStyleNode*>(div_node->inlineStyles.at("color").get());
    REQUIRE(color_value != nullptr);
    REQUIRE(color_value->toString() == "black");

    REQUIRE(div_node->inlineStyles.count("font-size"));
    auto* font_size_value = dynamic_cast<StaticStyleNode*>(div_node->inlineStyles.at("font-size").get());
    REQUIRE(font_size_value != nullptr);
    REQUIRE(font_size_value->toString() == "16px");

    // Check that the direct style is also present.
    REQUIRE(div_node->inlineStyles.count("background-color"));
    auto* bg_color_value = dynamic_cast<StaticStyleNode*>(div_node->inlineStyles.at("background-color").get());
    REQUIRE(bg_color_value != nullptr);
    REQUIRE(bg_color_value->toString() == "red");
}

TEST_CASE("CHTL Parser for Compound Style Values", "[CHTLParser]") {
    std::string source = R"(
        div {
            style {
                border: 1px solid black;
            }
        }
    )";

    Lexer lexer(source);
    Parser parser(lexer, "test.chtl");
    auto ast = parser.parse();

    REQUIRE(ast.size() == 1);
    auto* div_node = dynamic_cast<ElementNode*>(ast[0].get());
    REQUIRE(div_node != nullptr);

    // Check that the border property was parsed correctly.
    REQUIRE(div_node->inlineStyles.count("border"));
    auto* border_value = dynamic_cast<StaticStyleNode*>(div_node->inlineStyles.at("border").get());
    REQUIRE(border_value != nullptr);

    // This verifies that the parser correctly reconstructed the value from
    // the tokens "1", "px", "solid", and "black" after the lexer fix.
    REQUIRE(border_value->toString() == "1px solid black");
}