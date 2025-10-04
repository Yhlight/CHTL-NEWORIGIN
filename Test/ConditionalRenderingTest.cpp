#include "gtest/gtest.h"
#include "../src/CHTL/CHTLParser/CHTLParser.h"
#include "../src/Util/HtmlGenerator/HtmlGenerator.h"
#include "../src/Util/StyleEvaluator/StyleEvaluator.h"
#include "../src/Util/AttributeAutomator/AttributeAutomator.h"

TEST(ConditionalRenderingTest, AppliesStylesFromIfBlockWhenConditionIsTrue) {
    std::string chtl_code = R"(
        div {
            id: "test-box";
            style {
                width: 100px;
            }
            if {
                condition: #test-box.width > 50px,
                display: block,
            } else {
                display: none,
            }
        }
    )";

    CHTLParser parser(chtl_code);
    parser.parse();
    auto* root = parser.getRoot();

    AttributeAutomator automator;
    automator.process(root);

    StyleEvaluator styleEvaluator;
    styleEvaluator.evaluate(root);

    HtmlGenerator htmlGenerator;
    std::string result = htmlGenerator.generate(root);

    std::string expected_output = R"(<div id="test-box" style="width: 100px; display: block;">
</div>
)";
    ASSERT_EQ(result, expected_output);
}

TEST(ConditionalRenderingTest, AppliesStylesFromElseBlockWhenConditionIsFalse) {
    std::string chtl_code = R"(
        div {
            id: "test-box";
            style {
                width: 40px;
            }
            if {
                condition: #test-box.width > 50px,
                display: block,
            } else {
                display: none,
            }
        }
    )";

    CHTLParser parser(chtl_code);
    parser.parse();
    auto* root = parser.getRoot();

    AttributeAutomator automator;
    automator.process(root);

    StyleEvaluator styleEvaluator;
    styleEvaluator.evaluate(root);

    HtmlGenerator htmlGenerator;
    std::string result = htmlGenerator.generate(root);

    std::string expected_output = R"(<div id="test-box" style="width: 40px; display: none;">
</div>
)";
    ASSERT_EQ(result, expected_output);
}