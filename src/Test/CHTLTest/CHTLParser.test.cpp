#include <gtest/gtest.h>
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLNode/BaseNode.h"
#include <vector>
#include <string>

using namespace CHTL;

TEST(CHTLParserTest, ParsesNestedElementsAndAttributes) {
    std::string source = R"(
        html {
            id: "root-element";
            body {
                class: main_content;
            }
        }
    )";

    CHTLParser parser;
    auto ast = parser.parse(source);

    ASSERT_NE(ast, nullptr);

    std::string expected_ast_string =
        "Element(html) [\n"
        "  Attribute(id: root-element)\n"
        "  Element(body) [\n"
        "    Attribute(class: main_content)\n"
        "  ]\n"
        "]";

    EXPECT_EQ(ast->toString(), expected_ast_string);
}
