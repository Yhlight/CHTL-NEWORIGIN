#ifndef CHTL_CUSTOM_ELEMENT_NODE_H
#define CHTL_CUSTOM_ELEMENT_NODE_H

#include "TemplateElementNode.h"
#include "NodeVisitor.h"
#include <string>
#include <vector>
#include <unordered_set>

class CustomElementNode : public TemplateElementNode {
public:
    explicit CustomElementNode(const std::string& name)
        : TemplateElementNode(name) {}

    void addDeletedElement(const std::string& elementName) {
        deletedElements.insert(elementName);
    }

    const std::unordered_set<std::string>& getDeletedElements() const {
        return deletedElements;
    }

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto clonedNode = std::make_unique<CustomElementNode>(getName());
        for (const auto& child : getChildren()) {
            clonedNode->addChild(child->clone());
        }
        for (const auto& deletedElem : deletedElements) {
            clonedNode->addDeletedElement(deletedElem);
        }
        return clonedNode;
    }

private:
    std::unordered_set<std::string> deletedElements;
};

#endif // CHTL_CUSTOM_ELEMENT_NODE_H