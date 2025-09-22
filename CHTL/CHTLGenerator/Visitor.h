#pragma once

// Forward declare the node types to avoid circular dependencies
class ElementNode;
class TextNode;

class RootNode;

class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(RootNode& node) = 0;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
};
