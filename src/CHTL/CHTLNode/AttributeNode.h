#pragma once

#include "BaseNode.h"
#include <string>
#include <sstream>

namespace CHTL {

/**
 * @class AttributeNode
 * @brief Represents a key-value attribute of an element (e.g., id: "main").
 */
class AttributeNode : public BaseNode {
public:
    AttributeNode(const std::string& key, const std::string& value)
        : key(key), value(value) {}

    std::string toString(int indent = 0) const override {
        std::stringstream ss;
        ss << std::string(indent, ' ') << "Attribute(" << key << ": " << value << ")";
        return ss.str();
    }

    std::string key;
    std::string value;
};

} // namespace CHTL
