#ifndef CHTL_BASENODE_H
#define CHTL_BASENODE_H

#include <iostream>
#include <string>
#include <memory>

namespace CHTL {

enum class NodeType {
    ELEMENT,
    TEXT,
    STYLE,
    SCRIPT,
    TEMPLATE,
    LITERAL,
    BINARY_OP,
    STYLE_PROPERTY,
};

class BaseNode {
public:
    explicit BaseNode(NodeType type) : type(type) {}
    virtual ~BaseNode() = default;
    NodeType getType() const { return type; }
    virtual void print(int indent = 0) const = 0;
    virtual std::unique_ptr<BaseNode> clone() const = 0;

protected:
    NodeType type;
};

} // namespace CHTL

#endif // CHTL_BASENODE_H