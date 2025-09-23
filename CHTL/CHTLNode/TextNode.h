#pragma once

#include "BaseNode.h"
#include <string>

struct TextNode : public BaseNode {
    std::string content;

    explicit TextNode(const std::string& content) : content(content) {}

    NodeType getType() const override { return NodeType::Text; }

    void print(int level = 0) const override {
        for (int i = 0; i < level; ++i) std::cout << "  ";
        std::cout << "TextNode (\"" << content << "\")" << std::endl;
    }
};
