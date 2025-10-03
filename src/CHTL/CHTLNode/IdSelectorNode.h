#pragma once

#include "BaseNode.h"
#include <string>

class IdSelectorNode : public BaseNode {
public:
    IdSelectorNode(const std::string& name) : name(name) {}

    NodeType getType() const override { return NodeType::IdSelector; }

    const std::string& getName() const {
        return name;
    }

private:
    std::string name;
};