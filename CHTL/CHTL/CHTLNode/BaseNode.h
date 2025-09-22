#ifndef CHTL_BASE_NODE_H
#define CHTL_BASE_NODE_H

#include <string>
#include <memory>
#include <vector>
#include "../CHTLLexer/Token.h"

namespace CHTL {

class Generator; // Forward declaration

class BaseNode {
public:
    virtual ~BaseNode() = default;
    // For debugging and testing
    virtual std::string toString() const = 0;
    // For code generation (Visitor pattern)
    virtual void accept(Generator& visitor) const = 0;

    Token token; // The first token of this node, useful for error messages
};

} // namespace CHTL

#endif // CHTL_BASE_NODE_H
