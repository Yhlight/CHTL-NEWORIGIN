#pragma once

#include "BaseNode.h"
#include "PropertyNode.h"
#include <vector>
#include <memory>
#include <string>
#include <map>

namespace CHTL {

class ConfigurationNode : public BaseNode {
public:
    std::string name;
    std::map<std::string, std::vector<std::shared_ptr<PropertyNode>>> nested_blocks;

    ConfigurationNode() {
        type = NodeType::NODE_CONFIG;
    }

    const std::vector<std::shared_ptr<PropertyNode>> getSettings() const {
        std::vector<std::shared_ptr<PropertyNode>> settings;
        for (const auto& child : children) {
            if (child->getType() == NodeType::NODE_PROPERTY) {
                settings.push_back(std::dynamic_pointer_cast<PropertyNode>(child));
            }
        }
        return settings;
    }

    void addNestedBlock(const std::string& blockName, const std::vector<std::shared_ptr<PropertyNode>>& properties) {
        nested_blocks[blockName] = properties;
    }
};

}