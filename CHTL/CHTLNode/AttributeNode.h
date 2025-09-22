#pragma once

#include "BaseNode.h"
#include <string>
#include <memory>

// Represents an attribute key-value pair, e.g., id: "my-id"
struct AttributeNode : public Statement {
    Token token; // The token for the attribute name (e.g., 'id')
    std::unique_ptr<Expression> value;

    // Constructor now only takes the name token. The value is set by the parser.
    AttributeNode(Token t) : token(std::move(t)), value(nullptr) {}

    std::string tokenLiteral() const override { return token.literal; }
    NodeType getType() const override { return NodeType::ATTRIBUTE_DECLARATION; }

    std::string toString() const override {
        std::string out = token.literal + ": ";
        if (value) {
            out += value->toString();
        }
        out += ";";
        return out;
    }
};
