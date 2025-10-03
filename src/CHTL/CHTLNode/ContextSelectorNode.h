#pragma once

#include "BaseNode.h"
#include <string>

class ContextSelectorNode : public BaseNode {
public:
    ContextSelectorNode(const std::string& selector) : selector(selector) {}

    NodeType getType() const override { return NodeType::ContextSelector; }

    const std::string& getSelector() const {
        return selector;
    }

private:
    std::string selector;
};