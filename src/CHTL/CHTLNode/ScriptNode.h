#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

// Represents a script { ... } block inside an element.
class ScriptNode : public BaseNode {
public:
    std::string content;
    std::vector<std::unique_ptr<BaseNode>> children;

    explicit ScriptNode(std::string content);

    NodeType getType() const override {
        return NodeType::Script;
    }
};