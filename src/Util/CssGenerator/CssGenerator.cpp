#include "CssGenerator.h"
#include "../../CHTL/CHTLNode/ElementNode.h"
#include "../../CHTL/CHTLNode/StyleNode.h"
#include "../../CHTL/CHTLNode/ContextSelectorNode.h"
#include "../../CHTL/CHTLNode/StylePropertyNode.h"
#include "../../CHTL/CHTLNode/ValueNode.h"
#include <iostream>
#include <sstream>

void CssGenerator::generate(BaseNode* root, GlobalStylesheet& stylesheet) {
    if (!root) return;
    traverse(root, nullptr, stylesheet);
}

void CssGenerator::traverse(BaseNode* node, ElementNode* parentElement, GlobalStylesheet& stylesheet) {
    if (!node) return;

    ElementNode* currentElement = (node->getType() == NodeType::Element) ? static_cast<ElementNode*>(node) : parentElement;

    if (node->getType() == NodeType::Style) {
        if (currentElement && currentElement != node) {
            std::string inlineStyleString;
            for (const auto& child : node->getChildren()) {
                if (child->getType() == NodeType::StyleProperty) {
                    auto* propNode = static_cast<StylePropertyNode*>(child.get());
                    const auto* valueNode = static_cast<const ValueNode*>(propNode->getValue());
                    std::string value = valueNode->getValue();
                    inlineStyleString += propNode->getName() + ": " + value + ";";
                }
            }
            if (!inlineStyleString.empty()) {
                auto attrs = currentElement->getAttributes();
                if (attrs.count("style")) {
                    inlineStyleString = attrs.at("style") + " " + inlineStyleString;
                }
                currentElement->setAttribute("style", inlineStyleString);
            }
        }
    }
    else if (node->getType() == NodeType::ContextSelector) {
        auto* contextNode = static_cast<ContextSelectorNode*>(node);
        if (currentElement) {
            std::string parentSelector;
            auto attrs = currentElement->getAttributes();
            if (attrs.count("id")) {
                parentSelector = "#" + attrs["id"];
            } else if (attrs.count("class")) {
                std::stringstream ss(attrs["class"]);
                std::string className;
                while (ss >> className) {
                    parentSelector += "." + className;
                }
            }

            if (!parentSelector.empty()) {
                std::string fullSelector = parentSelector + contextNode->getSelector().substr(1);

                std::string properties;
                const auto& children = contextNode->getChildren();
                for (size_t i = 0; i < children.size(); ++i) {
                    const auto& propChild = children[i];
                    if (propChild->getType() == NodeType::StyleProperty) {
                        auto* propNode = static_cast<StylePropertyNode*>(propChild.get());
                        const auto* valueNode = static_cast<const ValueNode*>(propNode->getValue());
                        std::string value = valueNode->getValue();
                        properties += propNode->getName() + ": " + value + ";";
                        if (i < children.size() - 1) {
                            properties += " ";
                        }
                    }
                }
                if (!properties.empty()) {
                     stylesheet.addRule(fullSelector, properties);
                }
            }
        }
    }

    for (const auto& child : node->getChildren()) {
        traverse(child.get(), currentElement, stylesheet);
    }
}