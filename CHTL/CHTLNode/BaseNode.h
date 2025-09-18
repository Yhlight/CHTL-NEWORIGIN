#pragma once

#include <memory>
#include <vector>

namespace CHTL {

// The base class for all nodes in the Abstract Syntax Tree (AST).
// It's an abstract class with a virtual destructor to ensure
// proper cleanup of derived node types.
class BaseNode {
public:
    virtual ~BaseNode() = default;
};

} // namespace CHTL
