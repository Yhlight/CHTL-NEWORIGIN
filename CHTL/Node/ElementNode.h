#pragma once

#include "BaseNode.h"
#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <utility> // For std::pair

namespace CHTL {

class ElementNode : public BaseNode {
public:
    std::string tagName;
    std::vector<std::pair<std::string, std::string>> attributes;
    std::vector<std::unique_ptr<BaseNode>> children;

    ElementNode(const std::string& name) : tagName(name) {}

    NodeType getType() const override {
        return NodeType::Element;
    }

    void addChild(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }

    void addAttribute(const std::string& key, const std::string& value) {
        attributes.push_back({key, value});
    }

    std::string toString(int indent = 0) const override {
        std::string ind(indent * 2, ' ');
        std::stringstream ss;
        ss << ind << "ElementNode(" << tagName;
        if (!attributes.empty()) {
            ss << " [";
            for (size_t i = 0; i < attributes.size(); ++i) {
                ss << attributes[i].first << "=\"" << attributes[i].second << "\"";
                if (i < attributes.size() - 1) ss << ", ";
            }
            ss << "]";
        }
        ss << ") {\n";
        for (const auto& child : children) {
            ss << child->toString(indent + 1);
        }
        ss << ind << "}\n";
        return ss.str();
    }
};

} // namespace CHTL
