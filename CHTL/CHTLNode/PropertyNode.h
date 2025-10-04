#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

    class PropertyNode : public BaseNode {
    public:
        PropertyNode(const std::string& key, const std::string& value) : key(key), value(value) {
            type = NodeType::NODE_PROPERTY;
        }

        const std::string& getKey() const { return key; }
        const std::string& getValue() const { return value; }

    private:
        std::string key;
        std::string value;
    };

}