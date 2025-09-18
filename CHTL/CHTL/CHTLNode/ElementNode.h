#pragma once

#include "Node.h"
#include <string>
#include <utility>
#include <vector>
#include <map>

class ElementNode : public Node {
public:
    // The token would be the identifier token, e.g., 'div'.
    ElementNode(Token token, std::string tagName)
        : m_token(std::move(token)), m_tagName(std::move(tagName)) {}

    std::string tokenLiteral() const override {
        return m_token.literal;
    }

    // A simple string representation for debugging.
    std::string toString() const override {
        std::string out;
        out.append(m_token.literal);

        // Append attributes for debugging.
        out.append(" [");
        for (const auto& attr : m_attributes) {
            out.append(" " + attr.first + "=\"" + attr.second + "\"");
        }
        out.append(" ]");

        // Append inline styles for debugging
        if (!m_inlineStyles.empty()) {
            out.append(" style={");
            for (const auto& style : m_inlineStyles) {
                out.append(" " + style.first + ": " + style.second + ";");
            }
            out.append(" }");
        }

        out.append(" { ");
        for (const auto& child : m_children) {
            out.append(child->toString());
        }
        out.append(" }");
        return out;
    }

    Token m_token;
    std::string m_tagName;
    std::vector<NodePtr> m_children;
    std::map<std::string, std::string> m_attributes;
    std::map<std::string, std::string> m_inlineStyles;
};
