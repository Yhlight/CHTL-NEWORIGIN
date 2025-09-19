#include "ExpressionParser.h"
#include <stdexcept>

namespace CHTL {

ExpressionParser::ExpressionParser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::unique_ptr<Expr> ExpressionParser::parse() {
    try {
        return expression();
    } catch (const std::runtime_error& e) {
        return nullptr;
    }
}

std::unique_ptr<Expr> ExpressionParser::expression() {
    return ternary();
}

std::unique_ptr<Expr> ExpressionParser::ternary() {
    auto expr = logicOr();
    if (match({TokenType::QUESTION})) {
        auto thenBranch = expression();
        if (!match({TokenType::COLON})) {
            throw std::runtime_error("Expect ':' after then branch of conditional expression.");
        }
        auto elseBranch = ternary();
        expr = std::make_unique<TernaryExpr>(std::move(expr), std::move(thenBranch), std::move(elseBranch));
    }
    return expr;
}

std::unique_ptr<Expr> ExpressionParser::logicOr() {
    auto expr = logicAnd();
    while (match({TokenType::PIPE_PIPE})) {
        Token op = previous();
        auto right = logicAnd();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> ExpressionParser::logicAnd() {
    auto expr = comparison();
    while (match({TokenType::AMPERSAND_AMPERSAND})) {
        Token op = previous();
        auto right = comparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> ExpressionParser::comparison() {
    auto expr = term();
    while (match({TokenType::GREATER, TokenType::LESS})) {
        Token op = previous();
        auto right = term();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> ExpressionParser::term() {
    auto expr = factor();
    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token op = previous();
        auto right = factor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> ExpressionParser::factor() {
    auto expr = unary();
    while (match({TokenType::SLASH, TokenType::STAR, TokenType::PERCENT})) {
        Token op = previous();
        auto right = unary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> ExpressionParser::unary() {
    if (match({TokenType::MINUS})) {
        Token op = previous();
        auto right = unary();
        return std::make_unique<UnaryExpr>(op, std::move(right));
    }
    return primary();
}

std::unique_ptr<Expr> ExpressionParser::primary() {
    if (match({TokenType::NUMBER_LITERAL, TokenType::STRING_LITERAL, TokenType::UNQUOTED_LITERAL})) {
        return std::make_unique<LiteralExpr>(previous());
    }
    if (match({TokenType::LEFT_PAREN})) {
        auto expr = expression();
        if (!match({TokenType::RIGHT_PAREN})) {
            throw std::runtime_error("Expect ')' after expression.");
        }
        return std::make_unique<GroupingExpr>(std::move(expr));
    }
    throw std::runtime_error("Expect expression.");
}

bool ExpressionParser::isAtEnd() {
    return current >= tokens.size();
}

Token ExpressionParser::peek() {
    return tokens[current];
}

Token ExpressionParser::previous() {
    return tokens[current - 1];
}

Token ExpressionParser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool ExpressionParser::check(TokenType type) {
    if (isAtEnd()) return false;
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

} // namespace CHTL