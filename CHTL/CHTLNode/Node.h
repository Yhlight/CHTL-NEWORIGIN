#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

enum class NodeType {
    Element,
    Text,
    StyleBlock,
    StyleProperty,
    TemplateDeclaration,
    TemplateInstantiation
};

class Node {
public:
    virtual ~Node() = default;
    virtual NodeType getType() const = 0;
};

class TextNode : public Node {
public:
    std::string content;

    NodeType getType() const override { return NodeType::Text; }
};

class StyleBlockNode : public Node {
public:
    std::string css_content;

    NodeType getType() const override { return NodeType::StyleBlock; }
};

class StylePropertyNode : public Node {
public:
    std::string key;
    std::string value;

    NodeType getType() const override { return NodeType::StyleProperty; }
};

enum class TemplateType {
    Style,
    Element,
    Var
};

class TemplateDeclarationNode : public Node {
public:
    TemplateType templateType;
    std::string name;
    std::vector<std::unique_ptr<Node>> body;

    NodeType getType() const override { return NodeType::TemplateDeclaration; }
};

class TemplateInstantiationNode : public Node {
public:
    TemplateType templateType;
    std::string name;

    NodeType getType() const override { return NodeType::TemplateInstantiation; }
};

class ElementNode : public Node {
public:
    std::string tagName;
    std::map<std::string, std::string> attributes;
    std::vector<std::unique_ptr<Node>> children;

    NodeType getType() const override { return NodeType::Element; }
};

} // namespace CHTL
