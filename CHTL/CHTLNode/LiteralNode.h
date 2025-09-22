#pragma once

#include "BaseNode.h"

// Represents a literal value like a string, number, or unquoted literal
struct LiteralNode : public Expression {
    Token token; // The token containing the literal value
    std::string value;

    LiteralNode(Token t) : token(std::move(t)), value(token.literal) {}

    std::string tokenLiteral() const override { return token.literal; }
    NodeType getType() const override { return NodeType::LITERAL; }

    std::string toString() const override {
        if (token.type == TokenType::STRING_LITERAL) {
            return "\"" + value + "\"";
        }
        return value;
    }
};
