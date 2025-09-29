#include "catch.hpp"
#include "../src/CHTL/CHTLLexer/Lexer.h"
#include "../src/CHTL/CHTLParser/Parser.h"
#include "../src/CHTL/CHTLNode/ElementNode.h"
#include "../src/CHTL/CHTLNode/StyleValue.h"
#include "../src/CHTL/CHTLNode/DynamicStyleNode.h" // This file doesn't exist yet

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