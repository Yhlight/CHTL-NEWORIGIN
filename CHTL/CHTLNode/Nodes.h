#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

// Forward declarations
struct ElementNode;
struct TextNode;

// Using std::variant for a visitor-pattern-friendly AST
// but for now, let's stick to a more traditional inheritance model
// for simplicity.

class Node {
public:
    virtual ~Node() = default;
};

using NodePtr = std::shared_ptr<Node>;

struct Attribute {
    std::string name;
    std::string value;
};

class StyleBlockNode; // Forward declaration

class ElementNode : public Node {
public:
    std::string tagName;
    std::vector<Attribute> attributes;
    std::shared_ptr<StyleBlockNode> styleBlock;
    std::vector<NodePtr> children;
};

class TextNode : public Node {
public:
    std::string text;
};

struct StyleProperty {
    std::string name;
    std::string value;
};

class CssRuleNode : public Node {
public:
    std::string selector;
    std::vector<StyleProperty> properties;
};

class StyleBlockNode : public Node {
public:
    std::vector<StyleProperty> inlineProperties;
    std::vector<std::shared_ptr<CssRuleNode>> rules;
};


// ... other node types to be added later
// CommentNode, TemplateNode, CustomNode, StyleNode, etc.

class ImportNode : public Node {
public:
    enum class ImportType { CHTL, CSS, JS, CJMOD, CONFIG };
    ImportType type;
    std::string path;
    // for 'as' clause
    std::string alias;
};

} // namespace CHTL
