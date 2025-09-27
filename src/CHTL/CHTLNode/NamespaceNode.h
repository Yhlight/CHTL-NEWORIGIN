#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>
#include <iostream> // For debug printing if needed later

class NamespaceNode : public BaseNode {
public:
    std::string name;
    std::vector<std::unique_ptr<BaseNode>> children;

    NamespaceNode(std::string name, std::vector<std::unique_ptr<BaseNode>> children)
        : name(std::move(name)), children(std::move(children)) {}

    NodeType getType() const override {
        return NodeType::Namespace;
    }
};