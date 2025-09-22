#pragma once

#include "BaseNode.h"
#include <string>

struct AttributeNode : public BaseNode {
    std::string key;
    std::string value;

    AttributeNode(const std::string& key, const std::string& value)
        : key(key), value(value) {}

    NodeType getType() const override { return NodeType::Attribute; }
};
