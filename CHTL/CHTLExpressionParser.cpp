#include "CHTLExpressionParser.h"
#include <stdexcept>
#include <iostream>

namespace CHTL {

ExpressionParser::ExpressionParser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::shared_ptr<ExprNode> ExpressionParser::parse() {
    return parsePrecedence(0);
}

Token ExpressionParser::advance() {
    if (!isAtEnd()) current++;
    return tokens[current - 1];
}

Token ExpressionParser::peek() {
    return tokens[current];
}

bool ExpressionParser::isAtEnd() {
    // The expression should be terminated by a semicolon or brace,
    // which are not part of the token slice given to this parser.
    // So we just check if we've consumed all tokens.
    return current >= tokens.size();
}

int ExpressionParser::getPrecedence() {
    if (isAtEnd()) return 0;
    switch (peek().type) {
        case TokenType::PLUS:
        case TokenType::MINUS:
            return 1;
        case TokenType::STAR:
        case TokenType::SLASH:
        case TokenType::PERCENT:
            return 2;
        case TokenType::STAR_STAR:
            return 3;
        default:
            return 0;
    }
}

std::shared_ptr<ExprNode> ExpressionParser::prefixParse() {
    Token token = advance();
    switch (token.type) {
        case TokenType::NUMBER: {
            auto node = std::make_shared<NumericLiteralNode>();
            node->value = std::stod(token.lexeme);
            // This is a simplification. It doesn't handle units like "px" yet.
            // The lexer needs to be updated to separate the number and the unit.
            // For now, let's assume unitless numbers.
            node->unit = "";
            // A quick hack for units: if next token is an identifier, it's the unit.
            if (!isAtEnd() && peek().type == TokenType::IDENTIFIER) {
                node->unit = advance().lexeme;
            }
            return node;
        }
        case TokenType::STRING:
        case TokenType::IDENTIFIER: {
            auto node = std::make_shared<StringLiteralNode>();
            node->value = token.lexeme;
            return node;
        }
        case TokenType::LEFT_PAREN: {
            auto expr = parsePrecedence(0);
            if (advance().type != TokenType::RIGHT_PAREN) {
                // Handle error: unclosed parenthesis
                throw std::runtime_error("Expected ')' after expression.");
            }
            return expr;
        }
        // Handle other prefix expressions like negation (-) later
        default:
            // This should not be reached if the grammar is correct
            throw std::runtime_error("Expected expression but found token.");
    }
}

// Helper to get precedence for a given token type
int getPrecedenceForToken(TokenType type) {
    switch (type) {
        case TokenType::PLUS:
        case TokenType::MINUS:
            return 1;
        case TokenType::STAR:
        case TokenType::SLASH:
        case TokenType::PERCENT:
            return 2;
        case TokenType::STAR_STAR:
            return 3;
        default:
            return 0;
    }
}

std::shared_ptr<ExprNode> ExpressionParser::infixParse(std::shared_ptr<ExprNode> left) {
    Token opToken = advance();
    auto node = std::make_shared<BinaryOpNode>();
    node->op = opToken.type;
    node->left = left;
    int precedence = getPrecedenceForToken(opToken.type);
    node->right = parsePrecedence(precedence);
    return node;
}


std::shared_ptr<ExprNode> ExpressionParser::parsePrecedence(int precedence) {
    std::shared_ptr<ExprNode> left = prefixParse();

    while (precedence < getPrecedence()) {
        left = infixParse(left);
    }

    return left;
}


} // namespace CHTL
