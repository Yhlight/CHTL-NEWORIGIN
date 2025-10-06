#include "ExpressionParser.h"
#include "../CHTLNode/LiteralExpressionNode.h"
#include "../CHTLNode/UnaryExpressionNode.h"
#include "../CHTLNode/BinaryExpressionNode.h"
#include "../CHTLNode/GroupingExpressionNode.h"
#include "../CHTLNode/VariableExpressionNode.h"
#include <stdexcept>

namespace CHTL {

ExpressionParser::ExpressionParser(const std::vector<Token>& tokens)
    : tokens(tokens), position(0) {}

std::shared_ptr<ExpressionNode> ExpressionParser::parse() {
    try {
        return expression();
    } catch (const std::runtime_error& e) {
        // Error handling can be improved here
        return nullptr;
    }
}

std::shared_ptr<ExpressionNode> ExpressionParser::expression() {
    return logical_or();
}

std::shared_ptr<ExpressionNode> ExpressionParser::logical_or() {
    auto expr = logical_and();
    while (match({TokenType::TOKEN_OR})) {
        Token op = previous();
        auto right = logical_and();
        expr = std::make_shared<BinaryExpressionNode>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<ExpressionNode> ExpressionParser::logical_and() {
    auto expr = equality();
    while (match({TokenType::TOKEN_AND})) {
        Token op = previous();
        auto right = equality();
        expr = std::make_shared<BinaryExpressionNode>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<ExpressionNode> ExpressionParser::equality() {
    auto expr = comparison();
    while (match({TokenType::TOKEN_NEQ, TokenType::TOKEN_EQ})) {
        Token op = previous();
        auto right = comparison();
        expr = std::make_shared<BinaryExpressionNode>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<ExpressionNode> ExpressionParser::comparison() {
    auto expr = term();
    while (match({TokenType::TOKEN_GT, TokenType::TOKEN_GTE, TokenType::TOKEN_LT, TokenType::TOKEN_LTE})) {
        Token op = previous();
        auto right = term();
        expr = std::make_shared<BinaryExpressionNode>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<ExpressionNode> ExpressionParser::term() {
    auto expr = factor();
    while (match({TokenType::TOKEN_MINUS, TokenType::TOKEN_PLUS})) {
        Token op = previous();
        auto right = factor();
        expr = std::make_shared<BinaryExpressionNode>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<ExpressionNode> ExpressionParser::factor() {
    auto expr = unary();
    while (match({TokenType::TOKEN_DIVIDE, TokenType::TOKEN_MULTIPLY})) {
        Token op = previous();
        auto right = unary();
        expr = std::make_shared<BinaryExpressionNode>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<ExpressionNode> ExpressionParser::unary() {
    if (match({TokenType::TOKEN_MINUS})) {
        Token op = previous();
        auto right = unary();
        return std::make_shared<UnaryExpressionNode>(op, right);
    }
    return primary();
}

std::shared_ptr<ExpressionNode> ExpressionParser::primary() {
    if (match({TokenType::TOKEN_NUMERIC_LITERAL, TokenType::TOKEN_STRING_LITERAL})) {
        return std::make_shared<LiteralExpressionNode>(previous().lexeme);
    }
    if (match({TokenType::TOKEN_IDENTIFIER})) {
        return std::make_shared<VariableExpressionNode>(previous().lexeme);
    }
    if (match({TokenType::TOKEN_LPAREN})) {
        auto expr = expression();
        if (peek().type == TokenType::TOKEN_RPAREN) {
            advance(); // consume ')'
            return std::make_shared<GroupingExpressionNode>(expr);
        }
        throw std::runtime_error("Expected ')' after expression.");
    }
    throw std::runtime_error("Expected expression.");
}

bool ExpressionParser::isAtEnd() {
    return peek().type == TokenType::TOKEN_EOF;
}

Token ExpressionParser::peek() {
    if (position >= tokens.size()) {
        static Token eof{TokenType::TOKEN_EOF, "", 0, 0};
        return eof;
    }
    return tokens[position];
}

Token ExpressionParser::previous() {
    return tokens[position - 1];
}

Token ExpressionParser::advance() {
    if (!isAtEnd()) {
        position++;
    }
    return previous();
}

bool ExpressionParser::check(TokenType type) {
    if (isAtEnd()) {
        return false;
    }
    return peek().type == type;
}

bool ExpressionParser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

}