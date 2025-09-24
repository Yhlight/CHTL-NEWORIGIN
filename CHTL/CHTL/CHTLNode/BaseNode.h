#ifndef CHTL_BASE_NODE_H
#define CHTL_BASE_NODE_H

#include <memory>

class Visitor; // Forward declaration

// The base class for all nodes in the Abstract Syntax Tree.
// It provides a virtual destructor to ensure proper cleanup of derived node types.
class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual void accept(Visitor& visitor) = 0;
};

#endif // CHTL_BASE_NODE_H
