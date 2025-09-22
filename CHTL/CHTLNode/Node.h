#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <variant>

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
    TEMPLATE_DEFINITION,
    TEMPLATE_USAGE
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

enum class TemplateType {
    STYLE,
    ELEMENT,
    VAR
};

enum TemplateModifier {
    IS_TEMPLATE,
    IS_CUSTOM
};

// Represents a [Template] @Style ... { ... } definition
class TemplateDefinitionNode : public BaseNode {
public:
    TemplateModifier modifier;
    TemplateType template_type;
    std::string name;

    using StyleBody = std::vector<CssProperty>;
    using ElementBody = std::vector<std::unique_ptr<BaseNode>>;
    using VarBody = std::vector<std::pair<std::string, std::string>>;
    std::variant<StyleBody, ElementBody, VarBody> body;

    NodeType getType() const override { return NodeType::TEMPLATE_DEFINITION; }
};

// Represents a usage of a template, e.g. @Element MyBox;
class TemplateUsageNode : public BaseNode {
public:
    TemplateType template_type;
    std::string name;

    NodeType getType() const override { return NodeType::TEMPLATE_USAGE; }
};
