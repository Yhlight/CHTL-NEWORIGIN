#pragma once

#include "Node.h"
#include <string>
#include <utility>

class CommentNode : public Node {
public:
    // The token would be the GENERATOR_COMMENT token.
    CommentNode(Token token, std::string value)
        : m_token(std::move(token)), m_value(std::move(value)) {}

    std::string tokenLiteral() const override {
        return m_token.literal;
    }

    // A string representation for debugging.
    std::string toString() const override {
        return "<!-- " + m_value + " -->";
    }

    Token m_token;
    std::string m_value;
};
