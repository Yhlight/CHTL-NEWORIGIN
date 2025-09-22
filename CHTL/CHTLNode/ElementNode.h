#pragma once

#include "BaseNode.h"
#include "AttributeNode.h"
#include <string>
#include <vector>
#include <memory>

// Represents an element, e.g., div { ... }
// An element is a statement.
struct ElementNode : public Statement {
    Token token; // The token for the tag name (e.g., 'div')
    std::string tagName;
    std::unique_ptr<Program> body; // The block of statements inside the braces

    ElementNode(Token token, std::string name)
        : token(std::move(token)), tagName(std::move(name)), body(std::make_unique<Program>()) {}

    std::string tokenLiteral() const override { return token.literal; }
    NodeType getType() const override { return NodeType::ELEMENT_DECLARATION; }

    std::string toString() const override {
        std::string out = tagName + " {\n";
        if (body) {
            out += body->toString();
        }
        out += "\n}";
        return out;
    }
};
