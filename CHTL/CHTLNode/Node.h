#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>

// Forward declaration
class BaseNode;

// Define a struct for attributes
struct Attribute {
    std::string key;
    std::string value;
};

// Enum to identify node types, useful for visitors or casting
enum class NodeType {
    BASE,
    ELEMENT,
    TEXT,
    COMMENT,
    STYLE,
    TEMPLATE_DEFINITION
};

// Define a struct for CSS properties
struct CssProperty {
    std::string key;
    std::optional<std::string> value;
};

// Define a struct for a single CSS rule (e.g., .my-class { ... })
struct StyleRule {
    std::string selector;
    std::vector<CssProperty> properties;
};

// Base class for all AST nodes
class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual NodeType getType() const { return NodeType::BASE; }
};

// Represents an element, e.g., div { ... }
class ElementNode : public BaseNode {
public:
    std::string tagName;
    std::vector<Attribute> attributes;
    std::vector<std::unique_ptr<BaseNode>> children;

    NodeType getType() const override { return NodeType::ELEMENT; }
};

// Represents a text node, e.g., text { "Hello" }
class TextNode : public BaseNode {
public:
    std::string content;

    NodeType getType() const override { return NodeType::TEXT; }
};

// Represents a comment to be rendered, e.g., # a comment
class CommentNode : public BaseNode {
public:
    std::string content;

    NodeType getType() const override { return NodeType::COMMENT; }
};

// Represents a style { ... } block
class StyleNode : public BaseNode {
public:
    std::vector<CssProperty> inline_properties;
    std::vector<StyleRule> style_rules;

    NodeType getType() const override { return NodeType::STYLE; }
};

enum TemplateModifier {
    IS_TEMPLATE,
    IS_CUSTOM
};

// Represents a [Template] @Style ... { ... } definition
class TemplateDefinitionNode : public BaseNode {
public:
    TemplateModifier modifier;
    std::string name;
    // For now, we only support Style templates, so we store CssProperty.
    // This would need to be more generic for @Element or @Var templates.
    std::vector<CssProperty> style_properties;

    NodeType getType() const override { return NodeType::TEMPLATE_DEFINITION; }
};
