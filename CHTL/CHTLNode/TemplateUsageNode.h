#pragma once

#include "BaseNode.h"
#include "TemplateDefinitionNode.h" // For the TemplateType enum
#include <string>
#include <memory>

#include <vector>
#include <memory>
#include "ElementSpecializationNode.h" // Include the new container node

struct TemplateUsageNode : public BaseNode {
    std::string name;
    TemplateType templateType;

    // For @Style specialization
    std::vector<std::string> deletedProperties;

    // For @Element specialization
    std::shared_ptr<ElementSpecializationNode> specialization;

    TemplateUsageNode(const std::string& name, TemplateType type)
        : name(name), templateType(type), specialization(nullptr) {}

    NodeType getType() const override { return NodeType::TemplateUsage; }

    void print(int level = 0) const override {
        for (int i = 0; i < level; ++i) std::cout << "  ";
        std::cout << "TemplateUsageNode (@" << name << ")" << std::endl;
        if (specialization) {
            specialization->print(level + 1);
        }
    }
};
