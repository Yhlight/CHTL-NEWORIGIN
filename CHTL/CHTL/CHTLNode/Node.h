#pragma once

#include "../CHTLLexer/Token.h"
#include <string>
#include <vector>
#include <memory>

class Node;

// Use smart pointers for AST nodes to simplify memory management.
using NodePtr = std::shared_ptr<Node>;

// Node is the base interface for all nodes in the AST.
class Node {
public:
    virtual ~Node() = default;

    // tokenLiteral returns the literal value of the token the node is associated with.
    // This is primarily used for debugging.
    virtual std::string tokenLiteral() const = 0;

    // toString provides a string representation of the node, useful for debugging the AST structure.
    virtual std::string toString() const = 0;
};
