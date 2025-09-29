#include "ExpressionParser.h"
#include "../ExpressionNode/LiteralNode.h"
#include "../ExpressionNode/BinaryOpNode.h"
#include "../ExpressionNode/ConditionalNode.h"
#include "../ExpressionNode/PropertyRefNode.h"
#include <stdexcept>

ExpressionParser::ExpressionParser(const std::vector<ExpressionToken>& tokens)
    : tokens(tokens) {}

std::unique_ptr<ExpressionBaseNode> ExpressionParser::parse() {
    return parseExpression();
}

// Main parsing loop using Pratt's method.
std::unique_ptr<ExpressionBaseNode> ExpressionParser::parseExpression(int precedence) {
    std::unique_ptr<ExpressionBaseNode> left = parsePrefix();

    while (precedence < getPrecedence(peek())) {
        const ExpressionToken& token = advance();
        left = parseInfix(std::move(left), token);
    }

    return left;
}

// Parses prefix expressions (e.g., literals, grouped expressions).
std::unique_ptr<ExpressionBaseNode> ExpressionParser::parsePrefix() {
    const ExpressionToken& token = advance();
    switch (token.type) {
        case ExpressionTokenType::Number:
        case ExpressionTokenType::String:
            return std::make_unique<LiteralNode>(token.value);
        case ExpressionTokenType::Identifier: {
            // This could be a property reference like 'box.width'
            if (peek().type == ExpressionTokenType::Dot) {
                std::string selector = token.value;
                advance(); // consume '.'
                const ExpressionToken& propToken = advance();
                if (propToken.type != ExpressionTokenType::Identifier) {
                    throw std::runtime_error("Expected property name after '.'");
                }
                return std::make_unique<PropertyRefNode>(selector, propToken.value);
            }
            // For now, treat standalone identifiers as literals (e.g., "red", "solid")
            return std::make_unique<LiteralNode>(token.value);
        }
        case ExpressionTokenType::OpenParen: {
            auto expr = parseExpression();
            if (advance().type != ExpressionTokenType::CloseParen) {
                throw std::runtime_error("Expected ')'");
            }
            return expr;
        }
        default:
            throw std::runtime_error("Unexpected token in prefix position: " + token.value);
    }
}

// Parses infix expressions (e.g., binary operators, conditional operator).
std::unique_ptr<ExpressionBaseNode> ExpressionParser::parseInfix(std::unique_ptr<ExpressionBaseNode> left, const ExpressionToken& token) {
    switch (token.type) {
        case ExpressionTokenType::Plus:
        case ExpressionTokenType::Minus:
        case ExpressionTokenType::Asterisk:
        case ExpressionTokenType::Slash:
        case ExpressionTokenType::Percent:
        case ExpressionTokenType::DoubleAsterisk:
        case ExpressionTokenType::LogicalAnd:
        case ExpressionTokenType::LogicalOr:
        case ExpressionTokenType::EqualsEquals:
        case ExpressionTokenType::NotEquals:
        case ExpressionTokenType::GreaterThan:
        case ExpressionTokenType::LessThan:
        case ExpressionTokenType::GreaterThanEquals:
        case ExpressionTokenType::LessThanEquals: {
            auto right = parseExpression(getPrecedence(token));
            return std::make_unique<BinaryOpNode>(std::move(left), token.value, std::move(right));
        }
        case ExpressionTokenType::QuestionMark: {
            auto trueExpr = parseExpression();
            if (advance().type != ExpressionTokenType::Colon) {
                throw std::runtime_error("Expected ':' for ternary operator.");
            }
            auto falseExpr = parseExpression();
            return std::make_unique<ConditionalNode>(std::move(left), std::move(trueExpr), std::move(falseExpr));
        }
        default:
            throw std::runtime_error("Unexpected token in infix position: " + token.value);
    }
}

const ExpressionToken& ExpressionParser::peek() const {
    return tokens[position];
}

const ExpressionToken& ExpressionParser::advance() {
    if (!isAtEnd()) {
        return tokens[position++];
    }
    return tokens.back(); // EndOfFile token
}

bool ExpressionParser::isAtEnd() const {
    return position >= tokens.size() || tokens[position].type == ExpressionTokenType::EndOfFile;
}

// Defines operator precedence for the Pratt parser.
int ExpressionParser::getPrecedence(const ExpressionToken& token) const {
    switch (token.type) {
        case ExpressionTokenType::QuestionMark:
            return 1;
        case ExpressionTokenType::LogicalOr:
            return 2;
        case ExpressionTokenType::LogicalAnd:
            return 3;
        case ExpressionTokenType::EqualsEquals:
        case ExpressionTokenType::NotEquals:
            return 4;
        case ExpressionTokenType::GreaterThan:
        case ExpressionTokenType::LessThan:
        case ExpressionTokenType::GreaterThanEquals:
        case ExpressionTokenType::LessThanEquals:
            return 5;
        case ExpressionTokenType::Plus:
        case ExpressionTokenType::Minus:
            return 6;
        case ExpressionTokenType::Asterisk:
        case ExpressionTokenType::Slash:
        case ExpressionTokenType::Percent:
            return 7;
        case ExpressionTokenType::DoubleAsterisk:
            return 8;
        default:
            return 0;
    }
}