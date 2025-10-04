#include "gtest/gtest.h"
#include "../src/CHTL/CHTLParser/CHTLParser.h"
#include "../src/Util/StyleEvaluator/StyleEvaluator.h"
#include "../src/Util/HtmlGenerator/HtmlGenerator.h"
#include "../src/CHTL/CHTLNode/RootNode.h"
#include "../src/CHTL/CHTLNode/ElementNode.h"
#include "../src/CHTL/CHTLNode/DynamicBlockNode.h"

TEST(DynamicRenderingTest, TransformsAndGeneratesDynamicBlock) {
    std::string chtl = R"(
        div {
            if {
                condition: {{window}}.width > 500,
                color: red;
                font-size: 16px;
            }
        }
    )";

    // 1. Parse
    CHTLParser parser(chtl);
    parser.parse();
    auto root = parser.releaseRoot();

    // 2. Evaluate Styles (which transforms the node)
    StyleEvaluator styleEvaluator;
    styleEvaluator.evaluate(root.get());

    // 3. Check AST transformation
    const auto* element = static_cast<const ElementNode*>(root->getChildren()[0].get());
    ASSERT_EQ(element->getChildren().size(), 1);
    const auto* dynamicNode = element->getChildren()[0].get();
    ASSERT_EQ(dynamicNode->getType(), NodeType::DynamicBlock);

    // 4. Generate HTML
    HtmlGenerator htmlGenerator;
    std::string result = htmlGenerator.generate(root.get());

    // 5. Verify HTML output
    std::cout << "Generated HTML for Dynamic Test: " << result << std::endl;
    std::string expected_styles = "data-chtl-styles='{\"color\":\"red\",\"font-size\":\"16px\"}'";
    std::string expected_condition = "data-chtl-dynamic-if=\"{{window}}.width>500\"";

    ASSERT_NE(result.find("<template"), std::string::npos);
    ASSERT_NE(result.find(expected_condition), std::string::npos);
    ASSERT_NE(result.find(expected_styles), std::string::npos);
}