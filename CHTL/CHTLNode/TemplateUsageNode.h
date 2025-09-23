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
        : BaseNode(NodeType::TemplateUsage), name(name), templateType(type), specialization(nullptr) {}
};
