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
    TemplateDefinition,
    TemplateUsage,
    Origin,
    CustomDefinition
};

struct BaseNode {
    virtual ~BaseNode() = default;
    virtual NodeType getType() const { return NodeType::Base; }
};
