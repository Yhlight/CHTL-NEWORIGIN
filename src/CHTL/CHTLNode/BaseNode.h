#pragma once

#include <memory>

namespace CHTL {

// Forward declaration for the Visitor pattern
class ASTVisitor;

// Enum to identify node types without RTTI
enum class NodeType {
    Attribute,
    Element,
    Text,
    Comment,
    // ... other node types to be added
};

class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual std::string accept(ASTVisitor& visitor) = 0;
    virtual NodeType getType() const = 0;
};

// Define a smart pointer for AST nodes for automatic memory management.
using NodePtr = std::unique_ptr<BaseNode>;

} // namespace CHTL
