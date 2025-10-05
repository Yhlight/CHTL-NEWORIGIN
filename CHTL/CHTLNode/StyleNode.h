#pragma once

#include "BaseNode.h"
#include "PropertyNode.h"
#include <vector>
#include <memory>

namespace CHTL {

    class StyleNode : public BaseNode {
    public:
        StyleNode() : BaseNode(NodeType::NODE_STYLE) {}

        std::vector<std::shared_ptr<PropertyNode>> getProperties() const {
            std::vector<std::shared_ptr<PropertyNode>> properties;
            for (const auto& child : getChildren()) {
                if(child->getType() == NodeType::NODE_PROPERTY) {
                    properties.push_back(std::dynamic_pointer_cast<PropertyNode>(child));
                }
            }
            return properties;
        }
    };

}