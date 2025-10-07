#pragma once

#include "Node.h"
#include <string>
#include <map>

class ElementNode : public Node {
public:
    ElementNode(const std::string& tagName) : tagName(tagName) {}

    std::string tagName;
    std::map<std::string, std::string> attributes;
    NodeList children;

    NodePtr clone() const override {
        auto new_node = std::make_shared<ElementNode>(tagName);
        new_node->attributes = this->attributes;
        for (const auto& child : children) {
            new_node->children.push_back(child->clone());
        }
        return new_node;
    }

    // --- Specialization Helpers ---

    std::shared_ptr<ElementNode> findChildByTag(const std::string& tag) {
        for (const auto& child : children) {
            if (auto element = std::dynamic_pointer_cast<ElementNode>(child)) {
                if (element->tagName == tag) {
                    return element;
                }
            }
        }
        return nullptr;
    }

    bool removeChildByTag(const std::string& tag) {
        for (auto it = children.begin(); it != children.end(); ++it) {
            if (auto element = std::dynamic_pointer_cast<ElementNode>(*it)) {
                if (element->tagName == tag) {
                    children.erase(it);
                    return true;
                }
            }
        }
        return false;
    }

    bool insertChildrenAfter(NodePtr targetNode, const NodeList& newChildren) {
        for (auto it = children.begin(); it != children.end(); ++it) {
            if (*it == targetNode) {
                children.insert(it + 1, newChildren.begin(), newChildren.end());
                return true;
            }
        }
        return false;
    }
};