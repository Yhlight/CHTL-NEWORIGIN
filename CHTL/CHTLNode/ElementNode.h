#ifndef CHTL_ELEMENT_NODE_H
#define CHTL_ELEMENT_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>

class ElementNode : public BaseNode {
public:
    explicit ElementNode(const std::string& tagName) : tagName(tagName) {}

    void addChild(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }

    void setAttribute(const std::string& key, const std::string& value) {
        attributes[key] = value;
    }

    std::string toString(int depth = 0) const override {
        std::stringstream ss;
        ss << std::string(depth * 2, ' ') << "<" << tagName;
        for (const auto& attr : attributes) {
            ss << " " << attr.first << "=\"" << attr.second << "\"";
        }
        ss << ">" << std::endl;

        for (const auto& child : children) {
            ss << child->toString(depth + 1);
        }

        ss << std::string(depth * 2, ' ') << "</" << tagName << ">" << std::endl;
        return ss.str();
    }

private:
    std::string tagName;
    std::map<std::string, std::string> attributes;
    std::vector<std::unique_ptr<BaseNode>> children;
};

#endif //CHTL_ELEMENT_NODE_H