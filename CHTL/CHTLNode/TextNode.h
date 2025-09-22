#pragma once

#include "BaseNode.h"
#include <string>
#include <memory>

// Represents a text block, e.g., text { "Hello, World!" }
struct TextNode : public Statement {
    Token token; // The 'text' token
    std::string value; // The string content of the text node

    TextNode(Token token, std::string val)
        : token(std::move(token)), value(std::move(val)) {}

    std::string tokenLiteral() const override { return token.literal; }
    NodeType getType() const override { return NodeType::TEXT_DECLARATION; }

    std::string toString() const override {
        return token.literal + " { \"" + value + "\" }";
    }
};
