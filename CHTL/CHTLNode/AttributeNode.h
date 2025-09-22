#pragma once

#include "Node.h"

class AttributeNode {
public:
    AttributeNode(const std::string& name, const std::string& value)
        : name(name), value(value) {}

    std::string getName() const { return name; }
    std::string getValue() const { return value; }

private:
    std::string name;
    std::string value;
};

using AttributeList = std::vector<std::unique_ptr<AttributeNode>>;
