#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

enum class NodeType {
    PROGRAM,
    ELEMENT_NODE,
    TEXT_NODE,
};

class Node {
public:
    virtual ~Node() = default;
    virtual std::string ToString() const = 0;
    virtual NodeType GetType() const = 0;
};

// A program is the root of every AST our parser produces.
class Program : public Node {
public:
    std::vector<std::unique_ptr<Node>> children;

    std::string ToString() const override;
    NodeType GetType() const override { return NodeType::PROGRAM; }
};

} // namespace CHTL

#endif // NODE_H
