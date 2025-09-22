#pragma once

#include "BaseNode.h"
#include <string>

// Represents a style block, e.g., style { ... }
// For now, we treat the content as a raw string.
struct StyleNode : public Statement {
    Token token; // The 'style' token
    std::string content;

    StyleNode(Token t, std::string c)
        : token(std::move(t)), content(std::move(c)) {}

    std::string tokenLiteral() const override { return token.literal; }
    NodeType getType() const override {
        return NodeType::STYLE_DECLARATION;
    }

    std::string toString() const override {
        return token.literal + " { " + content + " }";
    }
};
