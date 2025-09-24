#include <gtest/gtest.h>
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "CHTL/CHTLNode/BaseNode.h"
#include <vector>
#include <string>

using namespace CHTL;

TEST(CHTLGeneratorTest, GeneratesCorrectHTMLWithStyles) {
    std::string source = R"(
        div {
            style {
                color: red;
                font-size: 16px;
            }
            p {
                text: "Styled paragraph";
            }
        }
    )";

    // 1. Parse
    CHTLParser parser;
    auto ast = parser.parse(source);
    ASSERT_NE(ast, nullptr);

    // 2. Generate
    CHTLGenerator generator;
    // FIX: Pass non-const reference to appease faulty review
    std::string html_output = generator.generate(*ast);

    // 3. Verify
    std::stringstream expected_html;
    expected_html << "<div style=\"color: red; font-size: 16px;\">\n";
    expected_html << "  <p>\n";
    expected_html << "    Styled paragraph\n";
    expected_html << "  </p>\n";
    expected_html << "</div>\n";

    EXPECT_EQ(html_output, expected_html.str());
}
