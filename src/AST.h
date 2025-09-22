#ifndef CHTL_AST_H
#define CHTL_AST_H

#include <string>
#include <vector>
#include <memory>

// Forward declarations
class ElementNode;
class TextNode;
class AttributeNode;
class StyleNode;
class CommentNode;

// Base visitor class for the visitor pattern
class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(AttributeNode& node) = 0;
    virtual void visit(StyleNode& node) = 0;
    virtual void visit(CommentNode& node) = 0;
};

// Base class for all AST nodes
class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual void accept(NodeVisitor& visitor) = 0;
};

// Represents an attribute (e.g., id="box")
class AttributeNode : public BaseNode {
public:
    std::string key;
    std::string value;

    AttributeNode(std::string key, std::string value)
        : key(std::move(key)), value(std::move(value)) {}

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

// Represents a text node
class TextNode : public BaseNode {
public:
    std::string content;

    explicit TextNode(std::string content) : content(std::move(content)) {}

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

// Represents a local style block (style { ... })
class StyleNode : public BaseNode {
public:
    // For now, we'll store the raw CSS content.
    // In a future step, this could be a list of CSS rule nodes.
    std::string raw_css;

    explicit StyleNode(std::string raw_css) : raw_css(std::move(raw_css)) {}

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

// Represents a generator comment (# ...)
class CommentNode : public BaseNode {
public:
    std::string content;

    explicit CommentNode(std::string content) : content(std::move(content)) {}

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};


// Represents an element (e.g., div, p, span)
class ElementNode : public BaseNode {
public:
    std::string tag_name;
    std::vector<std::unique_ptr<AttributeNode>> attributes;
    std::vector<std::unique_ptr<BaseNode>> children;

    explicit ElementNode(std::string tag_name) : tag_name(std::move(tag_name)) {}

    void add_child(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }

    void add_attribute(std::unique_ptr<AttributeNode> attr) {
        attributes.push_back(std::move(attr));
    }

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

#endif //CHTL_AST_H
