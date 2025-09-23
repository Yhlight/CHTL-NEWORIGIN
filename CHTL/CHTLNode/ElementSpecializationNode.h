#pragma once

#include "BaseNode.h"
#include <vector>
#include <memory>

// This node acts as a container for all the specialization instructions
// applied to a single element usage.
struct ElementSpecializationNode : public BaseNode {
    std::vector<std::shared_ptr<BaseNode>> instructions;

    void addInstruction(const std::shared_ptr<BaseNode>& instruction) {
        instructions.push_back(instruction);
    }

    NodeType getType() const override { return NodeType::ElementSpecialization; }
};
