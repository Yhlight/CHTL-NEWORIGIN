#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

    class PropertyNode : public BaseNode {
    public:
        std::string name;
        std::string value;

        PropertyNode() : BaseNode(NodeType::NODE_PROPERTY) {}
        PropertyNode(const std::string& name, const std::string& value) : BaseNode(NodeType::NODE_PROPERTY), name(name), value(value) {}

        const std::string& getKey() const { return name; }
        const std::string& getValue() const { return value; }
    };

}