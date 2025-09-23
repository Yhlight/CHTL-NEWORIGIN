#pragma once

#include "BaseNode.h"
#include <string>

struct AttributeNode : public BaseNode {
    std::string key;
    std::string value;

    AttributeNode(const std::string& key, const std::string& value)
        : BaseNode(NodeType::Attribute), key(key), value(value) {}
};
