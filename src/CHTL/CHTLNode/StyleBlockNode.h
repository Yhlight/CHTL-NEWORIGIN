#pragma once

#include "BaseNode.h"
#include <vector>
#include <memory>
#include <sstream>

namespace CHTL {

/**
 * @class StyleBlockNode
 * @brief Represents a style { ... } block within an element.
 */
class StyleBlockNode : public BaseNode {
public:
    StyleBlockNode() = default;

    std::string toString(int indent = 0) const override {
        std::stringstream ss;
        std::string indentation(indent, ' ');
        ss << indentation << "StyleBlock [\n";

        for (const auto& rule : rules) {
            ss << rule->toString(indent + 2) << "\n";
        }

        ss << indentation << "]";
        return ss.str();
    }

    // This can now hold StylePropertyNode, SelectorBlockNode, etc.
    std::vector<std::unique_ptr<BaseNode>> rules;
};

} // namespace CHTL
