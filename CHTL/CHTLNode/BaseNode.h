#pragma once

#include <string>
#include <memory>
#include <vector>
#include <iostream>

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
    CustomDefinition,
    // For element specialization
    ElementSpecialization,
    DeleteElement,
    ModifyElement,
    InsertElement
};

struct BaseNode {
    virtual ~BaseNode() = default;
    virtual NodeType getType() const { return NodeType::Base; }
    virtual void print(int level = 0) const = 0;
};
