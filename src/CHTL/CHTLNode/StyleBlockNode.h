#pragma once

#include "BaseNode.h"
#include "StylePropertyNode.h"
#include <vector>
#include <memory>
#include <sstream>

namespace CHTL {

/**
 * @class StyleBlockNode
 * @brief Represents a style { ... } block within an element.
 *
 * It acts as a container for various style-related nodes, starting
 * with simple properties.
 */
class StyleBlockNode : public BaseNode {
public:
    StyleBlockNode() = default;

    std::string toString(int indent = 0) const override {
        std::stringstream ss;
        std::string indentation(indent, ' ');
        ss << indentation << "StyleBlock [\n";

        for (const auto& prop : properties) {
            ss << prop->toString(indent + 2) << "\n";
        }

        ss << indentation << "]";
        return ss.str();
    }

    std::vector<std::unique_ptr<StylePropertyNode>> properties;
};

} // namespace CHTL
