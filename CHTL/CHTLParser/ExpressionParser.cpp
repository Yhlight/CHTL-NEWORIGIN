#include "ExpressionParser.h"

ExpressionParser::ExpressionParser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::unique_ptr<Expr> ExpressionParser::parse() {
    return expression();
}

std::unique_ptr<Expr> ExpressionParser::expression() {
    return addition();
}

std::unique_ptr<Expr> ExpressionParser::addition() {
    std::unique_ptr<Expr> expr = primary();

    while (match({TokenType::PLUS, TokenType::MINUS})) {
        Token op = previous();
        std::unique_ptr<Expr> right = primary();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> ExpressionParser::primary() {
    if (match({TokenType::NUMBER})) {
        return std::make_unique<Literal>(previous());
    }

    if (match({TokenType::LEFT_PAREN})) {
        std::unique_ptr<Expr> expr = expression();
        // consume(RIGHT_PAREN, "Expect ')' after expression."); // Error handling to be added
        advance();
        return std::make_unique<Grouping>(std::move(expr));
    }

    // Should throw an error here
    return nullptr;
}

// --- Helper Methods ---
bool ExpressionParser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE || peek().type == TokenType::SEMICOLON;
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
