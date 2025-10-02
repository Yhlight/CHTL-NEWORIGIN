#pragma once

#include "BaseNode.h"
#include <string>

class StyleNode : public BaseNode {
public:
    StyleNode() = default;

    NodeType getType() const override { return NodeType::Style; }
};