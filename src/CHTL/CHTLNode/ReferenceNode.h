#pragma once

#include "ExpressionNode.h"
#include <string>

class ReferenceNode : public ExpressionNode {
public:
    ReferenceNode(const std::string& reference) : reference(reference) {}

    NodeType getType() const override { return NodeType::Reference; }

    const std::string& getReference() const {
        return reference;
    }

private:
    std::string reference;
};