#include "gtest/gtest.h"
#include "../src/CHTL/CHTLParser/CHTLParser.h"
#include "../src/Util/TemplateEvaluator/TemplateEvaluator.h"
#include "../src/CHTL/CHTLNode/RootNode.h"
#include "../src/CHTL/CHTLNode/ElementNode.h"
#include "../src/CHTL/CHTLNode/StyleNode.h"
#include "../src/CHTL/CHTLNode/StylePropertyNode.h"
#include "../src/CHTL/CHTLNode/ValueNode.h"
#include "../src/CHTL/CHTLNode/TemplateDefinitionNode.h"
#include <map>

// Helper to count properties in an element's style node
static int countStyleProperties(const ElementNode* element) {
    if (!element) return 0;
    for (const auto& child : element->getChildren()) {
        if (child->getType() == NodeType::Style) {
            return child->getChildren().size();
        }
    }
    return 0;
}

// Helper to find a specific style property and return its value
static std::string findStylePropertyValue(const ElementNode* element, const std::string& propName) {
    if (!element) return "";
     for (const auto& child : element->getChildren()) {
        if (child->getType() == NodeType::Style) {
            const auto* styleNode = static_cast<const StyleNode*>(child.get());
            for (const auto& styleChild : styleNode->getChildren()) {
                if (styleChild->getType() == NodeType::StyleProperty) {
                    const auto* propNode = static_cast<const StylePropertyNode*>(styleChild.get());
                    if (propNode->getName() == propName) {
                        const auto* valueNode = static_cast<const ValueNode*>(propNode->getValue());
                        return valueNode->getValue();
                    }
                }
            }
        }
    }
    return "";
}

TEST(TemplateEvaluatorTest, ExpandsSimpleStyleTemplate) {
    std::string chtl = R"(
        [Template] @Style BaseText {
            color: blue;
            font-size: 16px;
        }

        div {
            style {
                @Style BaseText;
            }
        }
    )";

    CHTLParser parser(chtl);
    parser.parse();
    auto root = parser.releaseRoot();

    TemplateEvaluator evaluator;
    evaluator.evaluate(root.get());

    const auto* element = static_cast<const ElementNode*>(root->getChildren()[1].get());
    ASSERT_EQ(countStyleProperties(element), 2);
    ASSERT_EQ(findStylePropertyValue(element, "color"), "blue");
    ASSERT_EQ(findStylePropertyValue(element, "font-size"), "16px");
}

TEST(TemplateEvaluatorTest, ExpandsTemplateWithOtherProperties) {
    std::string chtl = R"(
        [Template] @Style BaseBox {
            border: 1px solid black;
        }

        div {
            style {
                background-color: red;
                @Style BaseBox;
                margin: 10px;
            }
        }
    )";

    CHTLParser parser(chtl);
    parser.parse();
    auto root = parser.releaseRoot();

    TemplateEvaluator evaluator;
    evaluator.evaluate(root.get());

    const auto* element = static_cast<const ElementNode*>(root->getChildren()[1].get());
    ASSERT_EQ(countStyleProperties(element), 3);
    ASSERT_EQ(findStylePropertyValue(element, "background-color"), "red");
    ASSERT_EQ(findStylePropertyValue(element, "border"), "1px solid black");
    ASSERT_EQ(findStylePropertyValue(element, "margin"), "10px");
}

TEST(TemplateEvaluatorTest, IgnoresNotFoundTemplate) {
    std::string chtl = R"(
        div {
            style {
                @Style NotFoundTemplate;
                color: green;
            }
        }
    )";

    CHTLParser parser(chtl);
    parser.parse();
    auto root = parser.releaseRoot();

    TemplateEvaluator evaluator;
    evaluator.evaluate(root.get());

    const auto* element = static_cast<const ElementNode*>(root->getChildren()[0].get());
    ASSERT_EQ(countStyleProperties(element), 1);
    ASSERT_EQ(findStylePropertyValue(element, "color"), "green");
}