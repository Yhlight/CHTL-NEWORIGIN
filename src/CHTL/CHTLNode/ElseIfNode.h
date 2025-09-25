#pragma once

#include "BaseNode.h"
#include "StyleValue.h"
#include <map>

class ElseIfNode : public BaseNode {
public:
    StyleValue condition;
    std::map<std::string, StyleValue> properties;

    ElseIfNode() : BaseNode() {}

    NodeType getType() const override { return NodeType::ElseIf; }
};