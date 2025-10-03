#include "gtest/gtest.h"
#include "../src/CHTL/CHTLParser/CHTLParser.h"
#include "../src/CHTL/CHTLNode/ElementNode.h"
#include "../src/CHTL/CHTLNode/IfBehaviorNode.h"
#include "../src/Util/StyleEvaluator/StyleEvaluator.h"
#include "../src/CHTL/CHTLNode/StyleNode.h"
#include "../src/CHTL/CHTLNode/StylePropertyNode.h"
#include "../src/CHTL/CHTLNode/ValueNode.h"

class IfBehaviorTest : public ::testing::Test {
protected:
    CHTLParser createParser(const std::string& input) {
        return CHTLParser(input);
    }

    const StyleNode* findStyleNode(const ElementNode* element) {
        if (!element) return nullptr;
        for (const auto& child : element->getChildren()) {
            if (child->getType() == NodeType::Style) {
                return static_cast<const StyleNode*>(child.get());
            }
        }
        return nullptr;
    }
};

TEST_F(IfBehaviorTest, ParsesSimpleIfElseChain) {
    std::string chtl_code = R"(
        div {
            if { condition: self.width > 100px, color: "red"; }
            else if { condition: self.width > 50px, color: "blue"; }
            else { color: "green"; }
        }
    )";

    CHTLParser parser = createParser(chtl_code);
    parser.parse();
    BaseNode* root = parser.getRoot();

    ASSERT_NE(root, nullptr);
    auto* divNode = dynamic_cast<ElementNode*>(root->getChildren()[0].get());
    ASSERT_NE(divNode, nullptr);
    ASSERT_EQ(divNode->getChildren().size(), 1);

    auto* ifNode = dynamic_cast<IfBehaviorNode*>(divNode->getChildren()[0].get());
    ASSERT_NE(ifNode, nullptr);

    const auto* elseIfChain = ifNode->getElseIfChain();
    ASSERT_NE(elseIfChain, nullptr);

    const auto* elseNode = elseIfChain->getElseNode();
    ASSERT_NE(elseNode, nullptr);
}

TEST_F(IfBehaviorTest, EvaluatesTrueCondition) {
    std::string chtl_code = R"(
        div {
            width: 200px;
            if {
                condition: self.width > 100px,
                color: "red";
            }
        }
    )";

    CHTLParser parser = createParser(chtl_code);
    parser.parse();
    BaseNode* root = parser.getRoot();

    StyleEvaluator evaluator;
    evaluator.evaluate(root);

    ASSERT_NE(root, nullptr);
    auto* divNode = dynamic_cast<ElementNode*>(root->getChildren()[0].get());
    ASSERT_NE(divNode, nullptr);

    ASSERT_EQ(divNode->getChildren().size(), 1);
    const StyleNode* newStyleNode = findStyleNode(divNode);
    ASSERT_NE(newStyleNode, nullptr);

    ASSERT_EQ(newStyleNode->getChildren().size(), 1);
    auto* propNode = dynamic_cast<StylePropertyNode*>(newStyleNode->getChildren()[0].get());
    ASSERT_NE(propNode, nullptr);
    ASSERT_EQ(propNode->getName(), "color");
    ASSERT_EQ(static_cast<const ValueNode*>(propNode->getValue())->getValue(), "red");
}

TEST_F(IfBehaviorTest, EvaluatesFalseCondition) {
    std::string chtl_code = R"(
        div {
            width: 50px;
            if {
                condition: self.width > 100px,
                color: "red";
            }
        }
    )";

    CHTLParser parser = createParser(chtl_code);
    parser.parse();
    BaseNode* root = parser.getRoot();

    StyleEvaluator evaluator;
    evaluator.evaluate(root);

    auto* divNode = dynamic_cast<ElementNode*>(root->getChildren()[0].get());
    ASSERT_NE(divNode, nullptr);

    // The IfBehaviorNode should be gone, and no StyleNode should be added.
    ASSERT_EQ(divNode->getChildren().size(), 0);
}

TEST_F(IfBehaviorTest, EvaluatesElseBlock) {
    std::string chtl_code = R"(
        div {
            width: 50px;
            if { condition: self.width > 100px, color: "red"; }
            else { color: "blue"; }
        }
    )";

    CHTLParser parser = createParser(chtl_code);
    parser.parse();
    BaseNode* root = parser.getRoot();

    StyleEvaluator evaluator;
    evaluator.evaluate(root);

    auto* divNode = dynamic_cast<ElementNode*>(root->getChildren()[0].get());
    const StyleNode* newStyleNode = findStyleNode(divNode);
    ASSERT_NE(newStyleNode, nullptr);

    ASSERT_EQ(newStyleNode->getChildren().size(), 1);
    auto* propNode = dynamic_cast<StylePropertyNode*>(newStyleNode->getChildren()[0].get());
    ASSERT_EQ(propNode->getName(), "color");
    ASSERT_EQ(static_cast<const ValueNode*>(propNode->getValue())->getValue(), "blue");
}

TEST_F(IfBehaviorTest, EvaluatesElseIfBlock) {
    std::string chtl_code = R"(
        div {
            width: 75px;
            if { condition: self.width > 100px, color: "red"; }
            else if { condition: self.width > 50px, color: "blue"; }
            else { color: "green"; }
        }
    )";

    CHTLParser parser = createParser(chtl_code);
    parser.parse();
    BaseNode* root = parser.getRoot();

    StyleEvaluator evaluator;
    evaluator.evaluate(root);

    auto* divNode = dynamic_cast<ElementNode*>(root->getChildren()[0].get());
    const StyleNode* newStyleNode = findStyleNode(divNode);
    ASSERT_NE(newStyleNode, nullptr);

    ASSERT_EQ(newStyleNode->getChildren().size(), 1);
    auto* propNode = dynamic_cast<StylePropertyNode*>(newStyleNode->getChildren()[0].get());
    ASSERT_EQ(propNode->getName(), "color");
    ASSERT_EQ(static_cast<const ValueNode*>(propNode->getValue())->getValue(), "blue");
}