#pragma once

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

enum class NodeType {
    Base,
    Element,
    Text,
    Comment,
    Attribute,
    StyleBlock,
    Expression,
    TemplateDefinition,
    TemplateUsage,
    CustomDefinition,
    CustomUsage,
    Origin
    // Other node types will be added as the implementation progresses.
};

class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual NodeType getType() const { return NodeType::Base; }

    // A virtual function to convert the node to its string representation, useful for debugging and generation.
    virtual std::string toString(int indentation = 0) const = 0;
};

} // namespace CHTL
