#include "StyleEvaluator.h"
#include "../../CHTL/CHTLNode/ElementNode.h"
#include "../../CHTL/CHTLNode/IfBehaviorNode.h"
#include "../../CHTL/CHTLNode/StyleNode.h"
#include "../../CHTL/CHTLNode/StylePropertyNode.h"
#include "../../CHTL/CHTLNode/ValueNode.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include <sstream>

void StyleEvaluator::evaluate(BaseNode *root) {
    if (!root)
        return;
    this->astRoot = root;
    traverse(root, nullptr);
}

void StyleEvaluator::processElementNode(ElementNode *elementNode) {
    auto &children = elementNode->getMutableChildren();
    std::vector<std::unique_ptr<StylePropertyNode>> conditionalProperties;

    auto it = std::remove_if(
        children.begin(), children.end(), [&](auto &child) {
            if (child->getType() == NodeType::IfBehavior) {
                auto *ifNode = static_cast<IfBehaviorNode *>(child.get());
                bool conditionMet = false;

                if (evaluator.evaluate(ifNode->getCondition(), this->astRoot, elementNode) == "true") {
                    conditionMet = true;
                    conditionalProperties = ifNode->takeProperties();
                } else {
                    const ElseIfBehaviorNode *currentElseIf = ifNode->getElseIfChain();
                    while (currentElseIf) {
                        if (evaluator.evaluate(currentElseIf->getCondition(), this->astRoot, elementNode) == "true") {
                            conditionMet = true;
                            conditionalProperties = const_cast<ElseIfBehaviorNode *>(currentElseIf)->takeProperties();
                            break;
                        }
                        currentElseIf = currentElseIf->getNextElseIf();
                    }
                }

                if (!conditionMet && ifNode->getElseNode()) {
                    conditionalProperties = const_cast<ElseBehaviorNode *>(ifNode->getElseNode())->takeProperties();
                }
                return true; // Mark for removal
            }
            return false;
        });
    children.erase(it, children.end());

    // Find the primary StyleNode or create one if it doesn't exist.
    StyleNode* primaryStyleNode = nullptr;
    for (const auto& child : children) {
        if (child->getType() == NodeType::Style) {
            primaryStyleNode = static_cast<StyleNode*>(child.get());
            break;
        }
    }
    if (!primaryStyleNode && !conditionalProperties.empty()) {
        auto newStyleNode = std::make_unique<StyleNode>();
        primaryStyleNode = newStyleNode.get();
        elementNode->addChild(std::move(newStyleNode));
    }

    // Add the conditional properties to the primary style node.
    if (primaryStyleNode) {
        for (auto &prop : conditionalProperties) {
            primaryStyleNode->addChild(std::move(prop));
        }
    }

    // After processing, consolidate all styles into the element's attribute.
    std::stringstream style_ss;
     for (const auto& child : elementNode->getChildren()) {
        if (auto* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            for (const auto& style_child : styleNode->getChildren()) {
                if (auto* prop = dynamic_cast<StylePropertyNode*>(style_child.get())) {
                    if (auto* val = dynamic_cast<const ValueNode*>(prop->getValue())) {
                         style_ss << prop->getName() << ": " << val->getValue() << "; ";
                    }
                }
            }
        }
    }
    std::string final_style = style_ss.str();
    if (!final_style.empty()) {
        final_style.pop_back(); // Remove trailing space
        elementNode->setAttribute("style", final_style);
    }
}

void StyleEvaluator::traverse(BaseNode *node, ElementNode* containingElement) {
    if (!node)
        return;

    // First, traverse deeper to evaluate nested expressions.
    for (const auto &child : node->getChildren()) {
        if (node->getType() == NodeType::Element) {
            traverse(child.get(), static_cast<ElementNode*>(node));
        } else {
            traverse(child.get(), containingElement);
        }
    }

    // Now, evaluate the current node's properties if it has any.
    if (node->getType() == NodeType::StyleProperty) {
        auto *propNode = static_cast<StylePropertyNode *>(node);
        if (propNode->getValue() && propNode->getValue()->getType() != NodeType::Value) {
            std::string finalValue = evaluator.evaluate(propNode->getValue(), this->astRoot, containingElement);
            propNode->setValue(std::make_unique<ValueNode>(finalValue));
        }
    }

    // After children are processed, handle the element itself.
    if (node->getType() == NodeType::Element) {
        processElementNode(static_cast<ElementNode*>(node));
    }
}