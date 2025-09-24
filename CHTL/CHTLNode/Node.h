#ifndef CHTL_NODE_H
#define CHTL_NODE_H

#include <memory>
#include <vector>
#include <string>

namespace CHTL {

// Using the Visitor pattern for future code generation
class AstVisitor;

class Node {
public:
    virtual ~Node() = default;
    virtual void Accept(AstVisitor& visitor) = 0;
};

} // namespace CHTL

#endif // CHTL_NODE_H
