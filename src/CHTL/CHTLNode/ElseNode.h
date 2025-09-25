#pragma once

#include "BaseNode.h"
#include "StyleValue.h"
#include <map>

class ElseNode : public BaseNode {
public:
    std::map<std::string, StyleValue> properties;

    ElseNode() : BaseNode() {}

    NodeType getType() const override { return NodeType::Else; }
};