#pragma once

#include "BaseNode.h"
#include "AttributeNode.h"
#include "StyleBlockNode.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>

namespace CHTL {

/**
 * @class ElementNode
 * @brief Represents an element in the AST (e.g., div { ... }).
 */
class ElementNode : public BaseNode {
public:
    explicit ElementNode(const std::string& tagName) : tagName(tagName) {}

    std::string toString(int indent = 0) const override {
        std::stringstream ss;
        std::string indentation(indent, ' ');
        ss << indentation << "Element(" << tagName << ") [\n";

        for (const auto& attr : attributes) {
            ss << attr->toString(indent + 2) << "\n";
        }

        if (style) {
            ss << style->toString(indent + 2) << "\n";
        }

        for (const auto& child : children) {
            ss << child->toString(indent + 2) << "\n";
        }

        ss << indentation << "]";
        return ss.str();
    }

    std::string tagName;
    std::vector<std::unique_ptr<AttributeNode>> attributes;
    std::unique_ptr<StyleBlockNode> style; // A dedicated pointer for the style block
    std::vector<std::unique_ptr<BaseNode>> children;
};

} // namespace CHTL
