#include "gtest/gtest.h"
#include "../src/CHTL/CHTLParser/CHTLParser.h"
#include "../src/Util/StyleEvaluator/StyleEvaluator.h"
#include "../src/CHTL/CHTLNode/RootNode.h"
#include "../src/CHTL/CHTLNode/ElementNode.h"
#include "../src/CHTL/CHTLNode/StyleNode.h"
#include "../src/CHTL/CHTLNode/StylePropertyNode.h"
#include "../src/CHTL/CHTLNode/ValueNode.h"

// Helper function to find a style property in an element
static const StylePropertyNode* findStyleProperty(const ElementNode* element, const std::string& propName) {
    if (!element) return nullptr;

    for (const auto& child : element->getChildren()) {
        if (child->getType() == NodeType::Style) {
            const auto* styleNode = static_cast<const StyleNode*>(child.get());
            for (const auto& styleChild : styleNode->getChildren()) {
                if (styleChild->getType() == NodeType::StyleProperty) {
                    const auto* propNode = static_cast<const StylePropertyNode*>(styleChild.get());
                    if (propNode->getName() == propName) {
                        return propNode;
                    }
                }
            }
        }
    }
    return nullptr;
}


TEST(ConditionalBlockTest, SimpleIfTrue) {
    std::string chtl = R"(
        div {
            style {
                width: 100px;
            }
            if {
                condition: width > 50px,
                height: 200px;
            }
        }
    )";

    CHTLParser parser(chtl);
    parser.parse();
    auto root = parser.releaseRoot();

    StyleEvaluator evaluator;
    evaluator.evaluate(root.get());

    const auto* element = static_cast<const ElementNode*>(root->getChildren()[0].get());
    const auto* heightProp = findStyleProperty(element, "height");

    ASSERT_NE(heightProp, nullptr);
    const auto* valueNode = static_cast<const ValueNode*>(heightProp->getValue());
    ASSERT_EQ(valueNode->getValue(), "200px");
}

TEST(ConditionalBlockTest, SimpleIfFalse) {
    std::string chtl = R"(
        div {
            style {
                width: 30px;
            }
            if {
                condition: width > 50px,
                height: 200px;
            }
        }
    )";

    CHTLParser parser(chtl);
    parser.parse();
    auto root = parser.releaseRoot();

    StyleEvaluator evaluator;
    evaluator.evaluate(root.get());

    const auto* element = static_cast<const ElementNode*>(root->getChildren()[0].get());
    const auto* heightProp = findStyleProperty(element, "height");

    ASSERT_EQ(heightProp, nullptr);
}

TEST(ConditionalBlockTest, IfElseChain_IfIsTrue) {
    std::string chtl = R"(
        div {
            style {
                width: 100px;
            }
            if {
                condition: width > 50px,
                height: 111px;
            } else {
                height: 222px;
            }
        }
    )";

    CHTLParser parser(chtl);
    parser.parse();
    auto root = parser.releaseRoot();

    StyleEvaluator evaluator;
    evaluator.evaluate(root.get());

    const auto* element = static_cast<const ElementNode*>(root->getChildren()[0].get());
    const auto* heightProp = findStyleProperty(element, "height");

    ASSERT_NE(heightProp, nullptr);
    const auto* valueNode = static_cast<const ValueNode*>(heightProp->getValue());
    ASSERT_EQ(valueNode->getValue(), "111px");
}

TEST(ConditionalBlockTest, IfElseChain_ElseIsTrue) {
    std::string chtl = R"(
        div {
            style {
                width: 30px;
            }
            if {
                condition: width > 50px,
                height: 111px;
            } else {
                height: 222px;
            }
        }
    )";

    CHTLParser parser(chtl);
    parser.parse();
    auto root = parser.releaseRoot();

    StyleEvaluator evaluator;
    evaluator.evaluate(root.get());

    const auto* element = static_cast<const ElementNode*>(root->getChildren()[0].get());
    const auto* heightProp = findStyleProperty(element, "height");

    ASSERT_NE(heightProp, nullptr);
    const auto* valueNode = static_cast<const ValueNode*>(heightProp->getValue());
    ASSERT_EQ(valueNode->getValue(), "222px");
}

TEST(ConditionalBlockTest, IfElseIfElseChain) {
    std::string chtl = R"(
        div {
            style {
                width: 75px;
            }
            if {
                condition: width > 100px,
                height: 111px;
            } else if {
                condition: width > 50px,
                height: 222px;
            } else {
                height: 333px;
            }
        }
    )";

    CHTLParser parser(chtl);
    parser.parse();
    auto root = parser.releaseRoot();

    StyleEvaluator evaluator;
    evaluator.evaluate(root.get());

    const auto* element = static_cast<const ElementNode*>(root->getChildren()[0].get());
    const auto* heightProp = findStyleProperty(element, "height");

    ASSERT_NE(heightProp, nullptr);
    const auto* valueNode = static_cast<const ValueNode*>(heightProp->getValue());
    ASSERT_EQ(valueNode->getValue(), "222px");
}