#pragma once

#include <vector>
#include <memory>
#include <string>

// Forward-declaration of all node types and the visitor
class ElementNode;
class TextNode;
class StyleBlockNode;
class StylePropertyNode;
class TemplateDefinitionNode;
class TemplateUsageNode;
class ConfigurationNode;
class ConfigPropertyNode;
class NodeVisitor;

// Base class for all nodes in the AST
class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual void accept(NodeVisitor& visitor) = 0;

    std::vector<std::shared_ptr<BaseNode>> children;
};

// Visitor interface for operating on the AST
class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(StyleBlockNode& node) = 0;
    virtual void visit(StylePropertyNode& node) = 0;
    virtual void visit(TemplateDefinitionNode& node) = 0;
    virtual void visit(TemplateUsageNode& node) = 0;
    virtual void visit(ConfigurationNode& node) = 0;
    virtual void visit(ConfigPropertyNode& node) = 0;
};
