#pragma once

#include "BaseNode.h"
#include <string>

class ConfigPropertyNode : public BaseNode {
public:
    ConfigPropertyNode(const std::string& key, const std::string& value)
        : key(key), value(value) {}

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::string key;
    std::string value;
};
