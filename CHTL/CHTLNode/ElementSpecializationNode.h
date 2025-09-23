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

    void print(int level = 0) const override {
        for (int i = 0; i < level; ++i) std::cout << "  ";
        std::cout << "ElementSpecializationNode" << std::endl;
        for (const auto& instruction : instructions) {
            instruction->print(level + 1);
        }
    }
};
