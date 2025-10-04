#pragma once

#include "BaseNode.h"
#include "PropertyNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class RuleNode : public BaseNode {
public:
    RuleNode(const std::string& selector) : selector(selector) {
        type = NodeType::NODE_RULE; // I will need to add NODE_RULE to the NodeType enum
    }

    const std::string& getSelector() const { return selector; }

    const std::vector<std::shared_ptr<PropertyNode>> getProperties() const {
        std::vector<std::shared_ptr<PropertyNode>> properties;
        for (const auto& child : children) {
            if (child->getType() == NodeType::NODE_PROPERTY) {
                properties.push_back(std::dynamic_pointer_cast<PropertyNode>(child));
            }
        }
        return properties;
    }

private:
    std::string selector;
};

}