#include "StyleEvaluator.h"
#include "../../CHTL/CHTLNode/ElementNode.h"
#include "../../CHTL/CHTLNode/IfBehaviorNode.h"
#include "../../CHTL/CHTLNode/StyleNode.h"
#include "../../CHTL/CHTLNode/StylePropertyNode.h"
#include "../../CHTL/CHTLNode/ValueNode.h"
#include <algorithm>
#include <iostream>
#include <vector>

void StyleEvaluator::evaluate(BaseNode *root) {
    if (!root)
        return;
    this->astRoot = root;
    traverse(root, nullptr);
}

void StyleEvaluator::processElementNode(ElementNode *elementNode) {
    auto &children = elementNode->getMutableChildren();
    std::vector<std::unique_ptr<StyleNode>> stylesToAdd;

    auto it = std::remove_if(
        children.begin(), children.end(), [&](auto &child) {
            if (child->getType() == NodeType::IfBehavior) {
                auto *ifNode = static_cast<IfBehaviorNode *>(child.get());
                bool conditionMet = false;

                std::string result = evaluator.evaluate(ifNode->getCondition(), this->astRoot, elementNode);
                if (result == "true") {
                    conditionMet = true;
                    auto properties = ifNode->takeProperties();
                    if (!properties.empty()) {
                        auto styleNode = std::make_unique<StyleNode>();
                        for (auto &prop : properties) {
                            styleNode->addChild(std::move(prop));
                        }
                        stylesToAdd.push_back(std::move(styleNode));
                    }
                } else {
                    const ElseIfBehaviorNode *currentElseIf = ifNode->getElseIfChain();
                    while (currentElseIf) {
                        result = evaluator.evaluate(currentElseIf->getCondition(), this->astRoot, elementNode);
                        if (result == "true") {
                            conditionMet = true;
                            auto properties = const_cast<ElseIfBehaviorNode *>(currentElseIf)->takeProperties();
                            if (!properties.empty()) {
                                auto styleNode = std::make_unique<StyleNode>();
                                for (auto &prop : properties) {
                                    styleNode->addChild(std::move(prop));
                                }
                                stylesToAdd.push_back(std::move(styleNode));
                            }
                            break;
                        }
                        currentElseIf = currentElseIf->getNextElseIf();
                    }
                }

                if (!conditionMet && ifNode->getElseNode()) {
                    auto properties = const_cast<ElseBehaviorNode *>(ifNode->getElseNode())->takeProperties();
                    if (!properties.empty()) {
                        auto styleNode = std::make_unique<StyleNode>();
                        for (auto &prop : properties) {
                            styleNode->addChild(std::move(prop));
                        }
                        stylesToAdd.push_back(std::move(styleNode));
                    }
                }
                return true; // Mark for removal
            }
            return false;
        });

    children.erase(it, children.end());

    for (auto &styleNode : stylesToAdd) {
        elementNode->addChild(std::move(styleNode));
    }
}

void StyleEvaluator::traverse(BaseNode *node, ElementNode* containingElement) {
    if (!node)
        return;

    ElementNode* currentElement = containingElement;
    if (node->getType() == NodeType::Element) {
        currentElement = static_cast<ElementNode*>(node);
        processElementNode(currentElement);
    }

    if (node->getType() == NodeType::StyleProperty) {
        auto *propNode = static_cast<StylePropertyNode *>(node);
        if (propNode->getValue() && propNode->getValue()->getType() != NodeType::Value) {
            std::string finalValue = evaluator.evaluate(propNode->getValue(), this->astRoot, currentElement);
            propNode->setValue(std::make_unique<ValueNode>(finalValue));
        }
    }

    for (const auto &child : node->getChildren()) {
        traverse(child.get(), currentElement);
    }
}