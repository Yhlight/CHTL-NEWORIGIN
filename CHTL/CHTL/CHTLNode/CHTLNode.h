#ifndef CHTL_NODE_H
#define CHTL_NODE_H

#include <string>
#include <vector>
#include <memory>

// Forward declare the visitor class
class AstVisitor;

class CHTLNode {
public:
    virtual ~CHTLNode() = default;
    virtual void accept(AstVisitor& visitor) = 0;
};

#endif // CHTL_NODE_H
