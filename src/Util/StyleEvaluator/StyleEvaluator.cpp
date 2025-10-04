#include "StyleEvaluator.h"
#include "../../CHTL/CHTLNode/StylePropertyNode.h"
#include "../../CHTL/CHTLNode/ValueNode.h"
#include "../../CHTL/CHTLNode/ElementNode.h"
#include "../../CHTL/CHTLNode/ConditionalBlockNode.h"
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
            // This part of the logic is pre-existing and may need revisiting.
            // The context isn't being properly passed down here.
            // For now, passing nullptr for the context node.
            std::string finalValue = evaluator.evaluate(propNode->getValue(), this->astRoot, nullptr);
            propNode->setValue(std::make_unique<ValueNode>(finalValue));
        }
    }

    // Evaluate conditional blocks within an element
    if (node->getType() == NodeType::Element) {
        auto* elementNode = static_cast<ElementNode*>(node);
        auto& children = elementNode->getChildren();
        std::vector<std::unique_ptr<BaseNode>> propertiesToAdd;
        bool conditionMetInChain = false;

        for (const auto& child : children) {
            if (child->getType() == NodeType::ConditionalBlock) {
                auto* conditionalNode = static_cast<ConditionalBlockNode*>(child.get());
                std::string condType = conditionalNode->getConditionalType();

                if (condType == "if") {
                    conditionMetInChain = false; // Start of a new chain
                }

                if (conditionMetInChain) {
                    continue;
                }

                bool isTrue = false;
                if (condType == "else") {
                    isTrue = true;
                } else {
                    ExpressionParser exprParser(conditionalNode->getCondition());
                    auto exprAST = exprParser.parse();
                    std::string result = evaluator.evaluate(exprAST.get(), astRoot, elementNode);
                    if (result == "true" || result == "1") {
                        isTrue = true;
                    }
                }

                if (isTrue) {
                    conditionMetInChain = true;
                    auto& conditionalChildren = conditionalNode->getChildren();
                    for(auto& prop : conditionalChildren) {
                        propertiesToAdd.push_back(std::move(prop));
                    }
                    conditionalChildren.clear();
                }
            }
        }

        if (!propertiesToAdd.empty()) {
            StyleNode* styleNode = nullptr;
            for (const auto& child : children) {
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

        auto it = std::remove_if(children.begin(), children.end(), [](const std::unique_ptr<BaseNode>& child) {
            return child->getType() == NodeType::ConditionalBlock;
        });
        children.erase(it, children.end());
    }
}