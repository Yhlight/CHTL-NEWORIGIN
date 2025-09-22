#pragma once

#include "../CHTLLexer/Token.h"
#include <string>
#include <memory>
#include <vector>

// Forward declaration for Expression nodes which will be defined later
struct Expression;

// Enum to identify the type of a node without using dynamic_cast
enum class NodeType {
    // Statements
    ELEMENT_DECLARATION,
    TEXT_DECLARATION,
    ATTRIBUTE_DECLARATION,
    STYLE_DECLARATION,

    // Expressions (to be expanded later)
    IDENTIFIER,
    LITERAL,
};

// Base class for all nodes in the AST
struct Node {
    virtual ~Node() = default;
    virtual std::string tokenLiteral() const = 0;
    virtual std::string toString() const = 0;
    virtual NodeType getType() const = 0;
};

// Base class for all statement nodes
struct Statement : public Node {
    // Statements don't produce values, so they don't need an expressionNode method
};

// Base class for all expression nodes
struct Expression : public Node {
    // Expressions produce values, so they might have a method like this
};

// A program is just a sequence of statements
struct Program : public Node {
    std::vector<std::unique_ptr<Statement>> statements;

    std::string tokenLiteral() const override {
        if (!statements.empty()) {
            return statements[0]->tokenLiteral();
        }
        return "";
    }

    std::string toString() const override {
        std::string out;
        for (const auto& stmt : statements) {
            out += stmt->toString();
        }
        return out;
    }

    NodeType getType() const override {
        // Not a standard node type, could have its own
        return NodeType::ELEMENT_DECLARATION; // Placeholder
    }
};
