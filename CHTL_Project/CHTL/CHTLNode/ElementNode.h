#pragma once

#include "BaseNode.h"
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <sstream>

namespace CHTL {

class ElementNode : public BaseNode {
public:
    std::string tagName;
    std::map<std::string, std::string> attributes;
    std::vector<std::shared_ptr<BaseNode>> children;

    ElementNode(const std::string& name) : tagName(name) {}

    NodeType getType() const override { return NodeType::Element; }

    void addChild(std::shared_ptr<BaseNode> child) {
        children.push_back(child);
    }

    void setAttribute(const std::string& key, const std::string& value) {
        attributes[key] = value;
    }

    std::string toString(int indentation = 0) const override {
        std::stringstream ss;
        std::string indent(indentation, ' ');
        ss << indent << "<" << tagName;
        for (const auto& attr : attributes) {
            ss << " " << attr.first << "=\"" << attr.second << "\"";
        }

        if (children.empty()) {
            ss << " />" << std::endl;
        } else {
            ss << ">" << std::endl;
            for (const auto& child : children) {
                ss << child->toString(indentation + 2);
            }
            ss << indent << "</" << tagName << ">" << std::endl;
        }
        return ss.str();
    }
};

} // namespace CHTL
