#pragma once

#include <string>
#include <memory>
#include <vector>

// Forward declare node types to break circular dependencies
struct ElementNode;
struct TextNode;
struct CommentNode;
struct AttributeNode;

enum class NodeType {
    Base,
    Element,
    Text,
    Comment,
    Attribute,
    CssRule,
    TemplateDefinition,
    TemplateUsage,
    Origin,
    CustomDefinition,
    Import,
    Namespace,
    Configuration,
    // For element specialization
    ElementSpecialization,
    DeleteElement,
    ModifyElement,
    InsertElement
};

struct BaseNode {
    NodeType type;

    BaseNode(NodeType type) : type(type) {}
    virtual ~BaseNode() = default;
    NodeType getType() const { return type; }
    virtual void print(int level = 0) const {
        // Base implementation does nothing.
    }
};
