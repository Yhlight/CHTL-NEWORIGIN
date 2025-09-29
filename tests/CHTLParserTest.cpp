#include "catch.hpp"
#include "../src/CHTL/CHTLLexer/Lexer.h"
#include "../src/CHTL/CHTLParser/Parser.h"
#include "../src/CHTL/CHTLNode/ElementNode.h"
#include "../src/CHTL/CHTLNode/StyleValue.h"
#include "../src/CHTL/CHTLNode/DynamicStyleNode.h"
#include "../src/CHTL/CHTLNode/ResponsiveValueNode.h" // This file doesn't exist yet

TEST_CASE("CHTL Parser for Dynamic Attribute Conditional Expressions", "[CHTLParser]") {
    std::string source = R"(
        div {
            style {
                width: {{box}}->width > 2 ? 100px : 50px;
            }
        }
    )";

    Lexer lexer(source);
    Parser parser(lexer);
    auto ast = parser.parse();

    REQUIRE(ast.size() == 1);
    auto* div_node = dynamic_cast<ElementNode*>(ast[0].get());
    REQUIRE(div_node != nullptr);

    const auto& styles = div_node->getStyleValues();
    REQUIRE(styles.size() == 1);

    const auto& style_value = styles.at("width");
    REQUIRE(style_value != nullptr);

    auto* dynamic_node = dynamic_cast<DynamicStyleNode*>(style_value.get());
    REQUIRE(dynamic_node != nullptr);
    REQUIRE(dynamic_node->getExpression() == "{{box}}->width > 2 ? 100px : 50px");
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
    Parser parser(lexer);
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