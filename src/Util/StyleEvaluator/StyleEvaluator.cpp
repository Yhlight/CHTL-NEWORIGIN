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
#include <unordered_set>

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

    // Evaluate expressions in style properties first
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

        // --- Pass 1: Evaluate conditions and identify which blocks to activate ---
        std::unordered_set<ConditionalBlockNode*> blocksToActivate;
        bool conditionMetInChain = false;

        for (const auto& child : elementNode->getChildren()) {
            if (child->getType() != NodeType::ConditionalBlock) {
                continue;
            }

            auto* conditionalNode = static_cast<ConditionalBlockNode*>(child.get());
            const std::string& condition = conditionalNode->getCondition();

            // Skip dynamic conditions in this static evaluation pass
            if (condition.find("{{") != std::string::npos) {
                continue;
            }

            const std::string& condType = conditionalNode->getConditionalType();
            if (condType == "if") {
                conditionMetInChain = false; // Reset for a new if-chain
            }

            if (conditionMetInChain) {
                continue; // A previous if/else-if in the chain was true
            }

            bool isTrue = false;
            if (condType == "else") {
                isTrue = true;
            } else { // "if" or "else if"
                 if (!condition.empty()) {
                    ExpressionParser exprParser(condition);
                    auto exprAST = exprParser.parse();
                    if (exprAST) {
                        std::string result = evaluator.evaluate(exprAST.get(), astRoot, elementNode);
                        if (result == "true" || result == "1") {
                            isTrue = true;
                        }
                    }
                 }
            }

            if (isTrue) {
                conditionMetInChain = true;
                blocksToActivate.insert(conditionalNode);
            }
        }

        // --- Pass 2: Rebuild the element's children based on evaluation ---
        auto oldChildren = std::move(elementNode->getChildren());
        elementNode->getChildren().clear();
        std::vector<std::unique_ptr<BaseNode>> propertiesToAdd;

        for (auto& child : oldChildren) {
            if (child->getType() != NodeType::ConditionalBlock) {
                elementNode->addChild(std::move(child)); // Keep non-conditional nodes
                continue;
            }

            auto* conditionalNode = static_cast<ConditionalBlockNode*>(child.get());

            // Handle dynamic blocks by converting them
            if (conditionalNode->getCondition().find("{{") != std::string::npos) {
                auto dynamicNode = std::make_unique<DynamicBlockNode>(
                    conditionalNode->getConditionalType(),
                    conditionalNode->getCondition()
                );
                for (auto& prop : conditionalNode->getChildren()) {
                    dynamicNode->addChild(std::move(prop));
                }
                elementNode->addChild(std::move(dynamicNode));
                continue;
            }

            // Handle static blocks
            if (blocksToActivate.count(conditionalNode)) {
                // This block was true, so extract its properties
                for (auto& prop : conditionalNode->getChildren()) {
                    propertiesToAdd.push_back(std::move(prop));
                }
            }
            // If not activated, the static conditional block is discarded
        }

        // Add the collected properties to the element's style node
        if (!propertiesToAdd.empty()) {
            StyleNode* styleNode = nullptr;
            // Find the style node in the *rebuilt* list of children
            for (auto& child : elementNode->getChildren()) {
                if (child->getType() == NodeType::Style) {
                    styleNode = static_cast<StyleNode*>(child.get());
                    break;
                }
            }
            // If no style node exists, create one
            if (!styleNode) {
                auto newStyleNode = std::make_unique<StyleNode>();
                styleNode = newStyleNode.get();
                elementNode->addChild(std::move(newStyleNode));
            }
            // Add the properties from the evaluated conditional block
            for (auto& prop : propertiesToAdd) {
                styleNode->addChild(std::move(prop));
            }
        }
    }
}