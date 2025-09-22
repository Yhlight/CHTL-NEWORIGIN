#pragma once

#include <memory>
#include <vector>
#include <string>

// Forward declaration to resolve circular dependency
class ElementNode;
class TextNode;
class CommentNode;

// Using a visitor pattern can be very helpful for operating on the AST.
// We can define a visitor interface that the generator will implement.
class AstVisitor {
public:
    virtual ~AstVisitor() = default;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(CommentNode& node) = 0;
};

class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual void accept(AstVisitor& visitor) = 0;
};
