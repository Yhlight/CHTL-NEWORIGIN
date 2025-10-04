#pragma once

#include "BaseNode.h"
#include "PropertyNode.h"
#include <vector>
#include <memory>

namespace CHTL {

class ConfigurationNode : public BaseNode {
public:
    ConfigurationNode() {
        type = NodeType::NODE_CONFIG;
    }

    const std::vector<std::shared_ptr<PropertyNode>> getSettings() const {
        std::vector<std::shared_ptr<PropertyNode>> settings;
        for (const auto& child : children) {
            settings.push_back(std::dynamic_pointer_cast<PropertyNode>(child));
        }
        return settings;
    }
};

}