#ifndef CHTL_AST_H
#define CHTL_AST_H

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <variant>
#include "ExpressionAST.h"

// Forward declarations
class ProgramNode;
class ElementNode;
class TextNode;
class AttributeNode;
class StyleNode;
class CommentNode;
class TemplateStyleNode;
class TemplateElementNode;
class TemplateVarNode;
class StyleUsageNode;
class ElementUsageNode;
class NamespaceNode;
class ConfigurationNode;


// Base visitor class for the visitor pattern
class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;
    virtual void visit(ProgramNode& node) = 0;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(AttributeNode& node) = 0;
    virtual void visit(StyleNode& node) = 0;
    virtual void visit(CommentNode& node) = 0;
    virtual void visit(TemplateStyleNode& node) = 0;
    virtual void visit(TemplateElementNode& node) = 0;
    virtual void visit(TemplateVarNode& node) = 0;
    virtual void visit(StyleUsageNode& node) = 0;
    virtual void visit(ElementUsageNode& node) = 0;
    virtual void visit(NamespaceNode& node) = 0;
    virtual void visit(ConfigurationNode& node) = 0;
};

// Base class for all AST nodes
class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual void accept(NodeVisitor& visitor) = 0;
};

// The root node of the entire document
class ProgramNode : public BaseNode {
public:
    std::vector<std::unique_ptr<BaseNode>> children;

    void add_child(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

// Represents a [Configuration] block
class ConfigurationNode : public BaseNode {
public:
    std::vector<std::unique_ptr<AttributeNode>> settings;

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

// Represents a [Namespace] block
class NamespaceNode : public BaseNode {
public:
    std::string name;
    std::vector<std::unique_ptr<BaseNode>> children;

    explicit NamespaceNode(std::string name) : name(std::move(name)) {}

    void add_child(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

// Represents a @Style usage
class StyleUsageNode : public BaseNode {
public:
    std::string name;
    explicit StyleUsageNode(std::string name) : name(std::move(name)) {}

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

// Represents a @Element usage
class ElementUsageNode : public BaseNode {
public:
    std::string name;
    explicit ElementUsageNode(std::string name) : name(std::move(name)) {}

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

// Represents a [Template] or [Custom] @Style definition
class TemplateStyleNode : public BaseNode {
public:
    std::string name;
    bool is_custom;
    std::vector<std::unique_ptr<AttributeNode>> styles;

    explicit TemplateStyleNode(std::string name, bool is_custom = false)
        : name(std::move(name)), is_custom(is_custom) {}

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

// Represents a [Template] or [Custom] @Element definition
class TemplateElementNode : public BaseNode {
public:
    std::string name;
    bool is_custom;
    std::vector<std::unique_ptr<BaseNode>> children;

    explicit TemplateElementNode(std::string name, bool is_custom = false)
        : name(std::move(name)), is_custom(is_custom) {}

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

// Represents a [Template] or [Custom] @Var definition
class TemplateVarNode : public BaseNode {
public:
    std::string name;
    bool is_custom;
    std::vector<std::unique_ptr<AttributeNode>> variables;

    explicit TemplateVarNode(std::string name, bool is_custom = false)
        : name(std::move(name)), is_custom(is_custom) {}

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};

// Represents an attribute (e.g., id="box")
class AttributeNode : public BaseNode {
public:
    using ValueType = std::variant<std::optional<std::string>, std::unique_ptr<ExprNode>>;

    std::string key;
    ValueType value;

    AttributeNode(std::string key, ValueType value)
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
