#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <utility>

namespace CHTL {

class AttributeNode : public BaseNode {
public:
    std::string name;
    std::string value;

    AttributeNode(std::string n, std::string v) : name(std::move(n)), value(std::move(v)) {}

    std::string toString() const override {
        return name + ": " + value + ";";
    }
};

class TextNode : public BaseNode {
public:
    std::string content;

    explicit TextNode(std::string c) : content(std::move(c)) {}

    std::string toString() const override {
        return "text { \"" + content + "\" }";
    }
};

class ElementNode : public BaseNode {
public:
    std::string tagName;
    std::vector<std::unique_ptr<AttributeNode>> attributes;
    std::vector<std::unique_ptr<BaseNode>> children;

    explicit ElementNode(std::string tag) : tagName(std::move(tag)) {}

    std::string toString() const override {
        std::stringstream ss;
        ss << tagName << " {\n";
        for (const auto& attr : attributes) {
            ss << "  " << attr->toString() << "\n";
        }
        for (const auto& child : children) {
            // This could get messy with deep nesting, but it's for debug.
            ss << "  " << child->toString() << "\n";
        }
        ss << "}";
        return ss.str();
    }
};

} // CHTL
