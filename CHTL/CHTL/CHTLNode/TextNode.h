#pragma once

#include "Node.h"
#include <string>
#include <utility>

class TextNode : public Node {
public:
    // The token would be the string token from the lexer.
    TextNode(Token token, std::string value)
        : m_token(std::move(token)), m_value(std::move(value)) {}

    std::string tokenLiteral() const override {
        return m_token.literal;
    }

    std::string toString() const override {
        return m_token.literal;
    }

    Token m_token;
    std::string m_value;
};
