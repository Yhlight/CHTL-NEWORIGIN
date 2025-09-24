#include <gtest/gtest.h>
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "CHTL/CHTLNode/BaseNode.h"
#include <vector>
#include <string>

using namespace CHTL;

TEST(CHTLGeneratorTest, GeneratesCorrectHTML) {
    // FIX: Update test case to validate text node parsing and generation
    std::string source = R"(
        html {
            body {
                p {
                    text: "Hello CHTL";
                }
            }
        }
    )";

    // 1. Parse
    CHTLParser parser;
    auto ast = parser.parse(source);
    ASSERT_NE(ast, nullptr);

    // 2. Generate
    CHTLGenerator generator;
    std::string html_output = generator.generate(*ast);

    // 3. Verify
    std::stringstream expected_html;
    expected_html << "<html>\n";
    expected_html << "  <body>\n";
    expected_html << "    <p>\n";
    expected_html << "      Hello CHTL\n";
    expected_html << "    </p>\n";
    expected_html << "  </body>\n";
    expected_html << "</html>\n";

    EXPECT_EQ(html_output, expected_html.str());
}
