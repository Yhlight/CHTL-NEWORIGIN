#include <gtest/gtest.h>
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "CHTL/CHTLNode/BaseNode.h"
#include <vector>
#include <string>

using namespace CHTL;

TEST(CHTLGeneratorTest, EndToEndStyleParsingAndGeneration) {
    std::string source = R"(
        html {
            head {}
            body {
                style {
                    color: blue;
                    .nested {
                        font-weight: bold;
                    }
                }
                p {
                    text: "Hello";
                }
            }
        }
    )";

    CHTLParser parser;
    auto ast = parser.parse(source);
    ASSERT_NE(ast, nullptr);

    CHTLGenerator generator;
    std::string body_html = generator.generate(*ast);
    std::string global_styles = generator.getGlobalStyles();

    // The test will now compose the final output itself.
    std::stringstream final_html_ss;
    final_html_ss << "<html>\n";
    final_html_ss << "  <head>\n";
    if (!global_styles.empty()) {
        final_html_ss << "    <style>\n";
        final_html_ss << global_styles;
        final_html_ss << "    </style>\n";
    }
    final_html_ss << "  </head>\n";
    // This is a simplification; we are assuming the root is always html
    // and we are grabbing its children's generated code.
    // A more robust test would traverse the AST.
    // For now, let's just test the generator output directly.

    std::string expected_body_html =
        "<html>\n"
        "  <head>\n"
        "  </head>\n"
        "  <body style=\"color: blue;\">\n"
        "    <p>\n"
        "      Hello\n"
        "    </p>\n"
        "  </body>\n"
        "</html>\n";

    std::string expected_global_styles =
        "    .nested {\n"
        "      font-weight: bold;\n"
        "    }\n";

    EXPECT_EQ(body_html, expected_body_html);
    EXPECT_EQ(global_styles, expected_global_styles);
}
