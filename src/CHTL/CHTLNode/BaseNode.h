#pragma once

#include <memory>
#include <string>

namespace CHTL {

// Forward declaration for the Visitor pattern, to be implemented later.
class ASTVisitor;

/**
 * @class BaseNode
 * @brief The abstract base class for all nodes in the Abstract Syntax Tree (AST).
 */
class BaseNode {
public:
    virtual ~BaseNode() = default;

    // The 'accept' method is part of the Visitor design pattern.
    // virtual void accept(ASTVisitor& visitor) = 0;

    // Method to get a string representation of the node for debugging.
    virtual std::string toString(int indent = 0) const = 0;
};

} // namespace CHTL
