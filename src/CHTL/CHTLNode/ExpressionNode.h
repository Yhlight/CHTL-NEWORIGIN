#pragma once

#include "BaseNode.h"

class ExpressionNode : public BaseNode {
public:
    // This class is abstract and serves as a base for expression-related nodes.
    virtual ~ExpressionNode() = default;
};