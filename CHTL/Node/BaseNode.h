#pragma once

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

enum class NodeType {
    Element,
    Text,
    Style,
    TemplateUsage,
    Origin,
    If,
    // ... other node types to be added later
};

class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual NodeType getType() const = 0;
    // Using a visitor pattern is a robust approach for generation, but a simple virtual method is sufficient for now.
    virtual std::string toString(int indent = 0) const = 0;
};

} // namespace CHTL
