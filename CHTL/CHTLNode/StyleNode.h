#pragma once

#include "BaseNode.h"
#include "PropertyNode.h"
#include <vector>
#include <memory>

namespace CHTL {

    class StyleNode : public BaseNode {
    public:
        StyleNode() {
            type = NodeType::NODE_STYLE;
        }

        const std::vector<std::shared_ptr<PropertyNode>>& getProperties() const {
            // This is not ideal, but it's a simple way to expose the properties
            // A better solution would be to have a dedicated list of properties
            // and not use the generic children from BaseNode.
            // For now, we'll cast the children.
            static std::vector<std::shared_ptr<PropertyNode>> properties;
            properties.clear();
            for (const auto& child : children) {
                properties.push_back(std::dynamic_pointer_cast<PropertyNode>(child));
            }
            return properties;
        }
    };

}