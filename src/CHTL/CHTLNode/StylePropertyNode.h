#pragma once

#include "BaseNode.h"
#include <string>
#include <sstream>

namespace CHTL {

/**
 * @class StylePropertyNode
 * @brief Represents a single CSS property within a style block (e.g., color: red;).
 */
class StylePropertyNode : public BaseNode {
public:
    StylePropertyNode(const std::string& property, const std::string& value)
        : property(property), value(value) {}

    std::string toString(int indent = 0) const override {
        std::stringstream ss;
        ss << std::string(indent, ' ') << "StyleProperty(" << property << ": " << value << ")";
        return ss.str();
    }

    std::string property;
    std::string value;
};

} // namespace CHTL
