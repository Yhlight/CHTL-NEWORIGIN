#include "StyleEvaluator.h"
#include "../../CHTL/CHTLNode/StylePropertyNode.h"
#include "../../CHTL/CHTLNode/ValueNode.h"
#include "../../CHTL/CHTLNode/ElementNode.h"
#include "../../CHTL/CHTLNode/ConditionalBlockNode.h"
#include "../../CHTL/CHTLNode/DynamicBlockNode.h"
#include "../../CHTL/CHTLNode/StyleNode.h"
#include "../../CHTL/ExpressionParser/ExpressionParser.h"
#include <vector>
#include <algorithm>

void StyleEvaluator::evaluate(BaseNode* root) {
    if (!root) return;
    this->astRoot = root;
    traverse(root);
}

void StyleEvaluator::traverse(BaseNode* node) {
    if (!node) return;

    // Post-order traversal: process children first
    for (const auto& child : node->getChildren()) {
        traverse(child.get());
    }

    // Evaluate expressions in style properties
    if (node->getType() == NodeType::StyleProperty) {
        auto* propNode = static_cast<StylePropertyNode*>(node);
        if (propNode->getValue() && propNode->getValue()->getType() != NodeType::Value) {
            std::string finalValue = evaluator.evaluate(propNode->getValue(), this->astRoot, nullptr);
            propNode->setValue(std::make_unique<ValueNode>(finalValue));
        }
    }

    // Evaluate conditional blocks within an element
    if (node->getType() == NodeType::Element) {
        auto* elementNode = static_cast<ElementNode*>(node);

        // Move children to a temporary vector to safely iterate and modify
        auto oldChildren = std::move(elementNode->getChildren());
        elementNode->getChildren().clear();

        std::vector<std::unique_ptr<BaseNode>> propertiesToAdd;
        bool conditionMetInChain = false;

        for (auto& child : oldChildren) {
            if (child->getType() != NodeType::ConditionalBlock) {
                elementNode->addChild(std::move(child));
                continue;
            }

            auto* conditionalNode = static_cast<ConditionalBlockNode*>(child.get());
            const std::string& condition = conditionalNode->getCondition();

            // Check for dynamic conditions
            if (condition.find("{{") != std::string::npos) {
                auto dynamicNode = std::make_unique<DynamicBlockNode>(
                    conditionalNode->getConditionalType(),
                    condition
                );
                auto& conditionalChildren = conditionalNode->getChildren();
                for (auto& prop : conditionalChildren) {
                    dynamicNode->addChild(std::move(prop));
                }
                elementNode->addChild(std::move(dynamicNode));
                continue;
            }

            // --- Static evaluation logic ---
            const std::string& condType = conditionalNode->getConditionalType();
            if (condType == "if") {
                conditionMetInChain = false;
            }

            if (conditionMetInChain) continue;

            bool isTrue = false;
            if (condType == "else") {
                isTrue = true;
            } else {
                ExpressionParser exprParser(condition);
                auto exprAST = exprParser.parse();
                if (exprAST) {
                    std::string result = evaluator.evaluate(exprAST.get(), astRoot, elementNode);
                    if (result == "true" || result == "1") {
                        isTrue = true;
                    }
                }
            }

            if (isTrue) {
                conditionMetInChain = true;
                auto& conditionalChildren = conditionalNode->getChildren();
                for(auto& prop : conditionalChildren) {
                    propertiesToAdd.push_back(std::move(prop));
                }
            }
        }

        // Add evaluated properties to the style node
        if (!propertiesToAdd.empty()) {
            StyleNode* styleNode = nullptr;
            for (const auto& child : elementNode->getChildren()) {
                if (child->getType() == NodeType::Style) {
                    styleNode = static_cast<StyleNode*>(child.get());
                    break;
                }
            }
            if (!styleNode) {
                auto newStyleNode = std::make_unique<StyleNode>();
                styleNode = newStyleNode.get();
                elementNode->addChild(std::move(newStyleNode));
            }
            for (auto& prop : propertiesToAdd) {
                styleNode->addChild(std::move(prop));
            }
        }
    }
}