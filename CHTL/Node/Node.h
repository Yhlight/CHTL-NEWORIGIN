#pragma once

#include <string>
#include <memory>
#include <vector>

// Forward declaration for the visitor pattern, which is a clean way to implement the generator
class AstVisitor;

class Node {
public:
    virtual ~Node() = default;
    virtual void accept(AstVisitor& visitor) const = 0;
    virtual std::string to_string(int indent = 0) const = 0;
};

// Define a unique_ptr for nodes to manage memory automatically
using NodePtr = std::unique_ptr<Node>;
using NodeList = std::vector<NodePtr>;
