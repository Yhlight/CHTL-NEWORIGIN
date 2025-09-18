#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "Expression/ExpressionNode.h"

namespace CHTL {

// Represents a single CSS-like property from a style block.
struct StyleProperty {
    std::string key;
    std::unique_ptr<ExpressionNode> value;
};

// A simple struct to hold key-value pairs for element attributes.
struct Attribute {
    std::string key;
    std::string value;
};

// Represents an element in the CHTL structure, like 'div' or 'span'.
class ElementNode : public BaseNode {
public:
    std::string tagName;
    std::vector<Attribute> attributes;
    std::vector<std::unique_ptr<BaseNode>> children;
    std::vector<StyleProperty> styles;

    void accept(NodeVisitor& visitor) const override {
        visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto new_node = std::make_unique<ElementNode>();
        new_node->tagName = this->tagName;
        new_node->attributes = this->attributes; // Attributes are simple structs, shallow copy is fine.

        // Deep copy styles
        for (const auto& style : this->styles) {
            new_node->styles.push_back(StyleProperty{style.key, style.value->clone()});
        }

        // Deep copy children
        for (const auto& child : this->children) {
            new_node->children.push_back(child->clone());
        }

        return new_node;
    }
};

} // namespace CHTL
