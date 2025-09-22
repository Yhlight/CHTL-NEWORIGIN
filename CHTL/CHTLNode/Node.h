#pragma once

#include <string>
#include <vector>
#include <memory>

// Forward declaration for the visitor pattern
class Visitor;

#include "../CHTLGenerator/Visitor.h"

class Node {
public:
    virtual ~Node() = default;
    // Using a visitor pattern is a clean way to implement the code generator
    // without cluttering the node classes with generation logic.
    virtual void accept(Visitor& visitor) = 0;
};

// Using smart pointers to manage the memory of the AST nodes.
using NodePtr = std::unique_ptr<Node>;
using NodeList = std::vector<NodePtr>;
