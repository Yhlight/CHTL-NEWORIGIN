#include "AttributeAutomator.h"
#include "../../CHTL/CHTLNode/ElementNode.h"
#include "../../CHTL/CHTLNode/StyleNode.h"
#include "../../CHTL/CHTLNode/ClassSelectorNode.h"
#include "../../CHTL/CHTLNode/IdSelectorNode.h"
#include <iostream>

void AttributeAutomator::process(BaseNode* root) {
    if (!root) return;
    traverse(root);
}

void AttributeAutomator::traverse(BaseNode* node) {
    if (!node) return;

    if (node->getType() == NodeType::Element) {
        auto* elementNode = static_cast<ElementNode*>(node);
        for (const auto& child : elementNode->getChildren()) {
            if (child->getType() == NodeType::Style) {
                auto* styleNode = static_cast<StyleNode*>(child.get());
                for (const auto& styleChild : styleNode->getChildren()) {
                    if (styleChild->getType() == NodeType::ClassSelector) {
                        auto* selectorNode = static_cast<ClassSelectorNode*>(styleChild.get());
                        auto attributes = elementNode->getAttributes();
                        std::string currentClass = attributes.count("class") ? attributes["class"] : "";
                        if (!currentClass.empty()) {
                            currentClass += " ";
                        }
                        currentClass += selectorNode->getName();
                        elementNode->setAttribute("class", currentClass);
                    } else if (styleChild->getType() == NodeType::IdSelector) {
                        auto* selectorNode = static_cast<IdSelectorNode*>(styleChild.get());
                        if (elementNode->getAttributes().find("id") == elementNode->getAttributes().end()) {
                            elementNode->setAttribute("id", selectorNode->getName());
                        }
                    }
                }
            }
        }
    }

    for (const auto& child : node->getChildren()) {
        traverse(child.get());
    }
}