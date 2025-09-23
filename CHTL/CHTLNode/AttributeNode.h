#pragma once

#include "BaseNode.h"
#include <string>

struct AttributeNode : public BaseNode {
    std::string key;
    std::string value;

    AttributeNode(const std::string& key, const std::string& value)
        : key(key), value(value) {}

    NodeType getType() const override { return NodeType::Attribute; }

    void print(int level = 0) const override {
        for (int i = 0; i < level; ++i) std::cout << "  ";
        std::cout << "AttributeNode (" << key << " = " << value << ")" << std::endl;
    }
};
