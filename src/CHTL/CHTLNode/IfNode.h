#pragma once

#include "BaseNode.h"
#include "StyleValue.h"
#include "ElseIfNode.h"
#include "ElseNode.h"
#include <map>
#include <vector>

class IfNode : public BaseNode {
public:
    StyleValue condition;
    std::map<std::string, StyleValue> properties;
    std::vector<std::unique_ptr<ElseIfNode>> elseIfNodes;
    std::unique_ptr<ElseNode> elseNode;

    IfNode() : BaseNode() {}

    NodeType getType() const override { return NodeType::If; }
};