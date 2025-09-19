#include "ExpressionParser.h"
#include <stdexcept>

namespace CHTL {

ExpressionParser::ExpressionParser(const std::vector<Token>& tokens)
    : tokens(tokens), current_token_index(0) {}

Token ExpressionParser::peek() {
    if (current_token_index >= tokens.size()) {
        return {TokenType::END_OF_FILE, "", 0, 0};
    }
    return tokens[current_token_index];
}

Token ExpressionParser::advance() {
    if (current_token_index < tokens.size()) {
        current_token_index++;
    }
    return tokens[current_token_index - 1];
}

// Entry point for the expression parser
std::unique_ptr<IExpressionNode> ExpressionParser::parse() {
    return parseExpression();
}

// Handles + and - (lowest precedence)
std::unique_ptr<IExpressionNode> ExpressionParser::parseExpression() {
    auto left = parseTerm();

    while (peek().type == TokenType::PLUS || peek().type == TokenType::MINUS) {
        TokenType op = advance().type;
        auto right = parseTerm();
        auto node = std::make_unique<BinaryOpNode>();
        node->left = std::move(left);
        node->op = op;
        node->right = std::move(right);
        left = std::move(node);
    }

    return left;
}

// Handles * and / (medium precedence)
std::unique_ptr<IExpressionNode> ExpressionParser::parseTerm() {
    auto left = parseFactor();

    while (peek().type == TokenType::STAR || peek().type == TokenType::SLASH) {
        TokenType op = advance().type;
        auto right = parseFactor();
        auto node = std::make_unique<BinaryOpNode>();
        node->left = std::move(left);
        node->op = op;
        node->right = std::move(right);
        left = std::move(node);
    }

    return left;
}

// Handles numbers and units
std::unique_ptr<IExpressionNode> ExpressionParser::parseFactor() {
    return parsePrimary();
}

// Handles the primary components of an expression
std::unique_ptr<IExpressionNode> ExpressionParser::parsePrimary() {
    if (peek().type == TokenType::NUMBER) {
        auto node = std::make_unique<NumberNode>();
        node->value = std::stod(advance().value);

        // Check for an optional unit
        if (peek().type == TokenType::IDENTIFIER) {
            node->unit = advance().value;
        }
        return node;
    }

    // For now, we only handle numbers.
    // Later, we can add support for parentheses or variables here.
    return nullptr;
}

} // namespace CHTL
