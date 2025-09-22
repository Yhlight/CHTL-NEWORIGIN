#ifndef CHTL_NODE_H
#define CHTL_NODE_H

#include "../CHTLLexer/Token.h"
#include <vector>
#include <string>
#include <memory>
#include <sstream>

namespace CHTL {

// Forward declarations
class Statement;
class StylePropertyNode;

// Base class for all nodes in the Abstract Syntax Tree.
class Node {
public:
    virtual ~Node() = default;
    virtual std::string tokenLiteral() const = 0;
    virtual std::string toString() const = 0;
};

// Base class for all statement nodes.
class Statement : public Node {
public:
    std::string tokenLiteral() const override { return ""; }
};

// The root node of a CHTL program.
class Program : public Node {
public:
    std::vector<std::unique_ptr<Statement>> statements;
    std::string tokenLiteral() const override { return "Program"; }
    std::string toString() const override {
        std::stringstream ss;
        for (const auto& stmt : statements) {
            ss << stmt->toString();
        }
        return ss.str();
    }
};

// Represents a single CSS property.
class StylePropertyNode : public Statement {
public:
    Token property;
    std::string value;
    std::string tokenLiteral() const override { return property.literal; }
    std::string toString() const override {
        std::stringstream ss;
        ss << property.literal << ": " << value << ";";
        return ss.str();
    }
};

// Represents the definition of a style template.
class StyleTemplateNode : public Statement {
public:
    Token name;
    std::vector<std::unique_ptr<StylePropertyNode>> properties;
    std::string tokenLiteral() const override { return "Template"; }
    std::string toString() const override {
        return "[Template] @Style " + name.literal + " { ... }";
    }
};

// Represents a delete statement.
class DeleteStatementNode : public Statement {
public:
    Token target;
    std::string tokenLiteral() const override { return "delete"; }
    std::string toString() const override {
        return "delete " + target.literal + ";";
    }
};

// Represents the usage of a style template.
class StyleTemplateUsageNode : public Statement {
public:
    Token name;
    Token fromNamespace;
    std::vector<std::unique_ptr<Statement>> body;
    std::string tokenLiteral() const override { return "@Style"; }
    std::string toString() const override {
        return "@Style " + name.literal + ";";
    }
};

// Represents the definition of an element template.
class ElementTemplateNode : public Statement {
public:
    Token name;
    std::vector<std::unique_ptr<Statement>> body;
    std::string tokenLiteral() const override { return "Template"; }
    std::string toString() const override {
        return "[Template] @Element " + name.literal + " { ... }";
    }
};

// Represents the usage of an element template.
class ElementTemplateUsageNode : public Statement {
public:
    Token name;
    Token fromNamespace;
    std::string tokenLiteral() const override { return "@Element"; }
    std::string toString() const override {
        return "@Element " + name.literal + ";";
    }
};

// Represents an import statement.
class ImportNode : public Statement {
public:
    Token type;
    Token path;
    std::string tokenLiteral() const override { return "Import"; }
    std::string toString() const override {
        return "[Import] " + type.literal + " from \"" + path.literal + "\"";
    }
};

// Represents a namespace block.
class NamespaceNode : public Statement {
public:
    Token name;
    std::vector<std::unique_ptr<Statement>> body;
    std::string tokenLiteral() const override { return "Namespace"; }
    std::string toString() const override {
        return "[Namespace] " + name.literal + " { ... }";
    }
};

// Represents a CSS rule with a selector.
class SelectorRuleNode : public Statement {
public:
    Token token;
    Token name;
    std::vector<std::unique_ptr<StylePropertyNode>> properties;
    std::string getSelector() const { return token.literal + name.literal; }
    std::string tokenLiteral() const override { return token.literal; }
    std::string toString() const override {
        return getSelector() + " { ... }";
    }
};

// Represents a style block.
class StyleBlockNode : public Statement {
public:
    std::vector<std::unique_ptr<Statement>> body;
    std::string tokenLiteral() const override { return "style"; }
    std::string toString() const override {
        return "style { ... }";
    }
};

// Represents an attribute key-value pair.
class AttributeNode : public Statement {
public:
    Token token;
    Token value;
    std::string tokenLiteral() const override { return token.literal; }
    std::string toString() const override {
        return token.literal + ": " + value.literal + ";";
    }
};

// Represents an element node.
class ElementNode : public Statement {
public:
    Token token;
    std::vector<std::unique_ptr<AttributeNode>> attributes;
    std::unique_ptr<StyleBlockNode> styleBlock;
    std::vector<std::unique_ptr<Statement>> children;
    std::string tokenLiteral() const override { return token.literal; }
    std::string toString() const override {
        return token.literal + " { ... }";
    }
};

// Represents a text node.
class TextNode : public Statement {
public:
    Token value;
    std::string tokenLiteral() const override { return "text"; }
    std::string toString() const override {
        return "text { " + value.literal + " }";
    }
};

} // namespace CHTL
#endif // CHTL_NODE_H
