#include "ExpressionParser.h"
#include "../CHTLNode/LiteralNode.h"
#include "../CHTLNode/BinaryOpNode.h"
#include <stdexcept>

namespace CHTL {

ExpressionParser::ExpressionParser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::unique_ptr<ExpressionNode> ExpressionParser::parse() {
    return addition();
}

// Grammar Rule: addition -> multiplication ( ( "-" | "+" ) multiplication )*
std::unique_ptr<ExpressionNode> ExpressionParser::addition() {
    std::unique_ptr<ExpressionNode> expr = multiplication();

    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token op = tokens[current - 1];
        std::unique_ptr<ExpressionNode> right = multiplication();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }

    return expr;
}

// Grammar Rule: multiplication -> primary ( ( "/" | "*" ) primary )*
std::unique_ptr<ExpressionNode> ExpressionParser::multiplication() {
    std::unique_ptr<ExpressionNode> expr = primary();

    while (match({TokenType::SLASH, TokenType::ASTERISK})) {
        Token op = tokens[current - 1];
        std::unique_ptr<ExpressionNode> right = primary();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }

    return expr;
}

// Grammar Rule: primary -> UNQUOTED_LITERAL | "(" expression ")"
std::unique_ptr<ExpressionNode> ExpressionParser::primary() {
    if (match({TokenType::UNQUOTED_LITERAL, TokenType::IDENTIFIER})) {
        return std::make_unique<LiteralNode>(tokens[current - 1]);
    }

    if (match({TokenType::LEFT_PAREN})) {
        std::unique_ptr<ExpressionNode> expr = parse();
        if (!match({TokenType::RIGHT_PAREN})) {
            throw std::runtime_error("Expect ')' after expression.");
        }
        return expr;
    }

    throw std::runtime_error("Expect expression.");
}


// Helper methods
bool ExpressionParser::isAtEnd() {
    // We consider the end of the token list as the end.
    return current >= tokens.size();
}

Token ExpressionParser::peek() {
    if (isAtEnd()) {
        // Return an EOF-like token to prevent out-of-bounds access
        return {TokenType::END_OF_FILE, "", -1, -1};
    }
    return tokens[current];
}

Token ExpressionParser::advance() {
    if (!isAtEnd()) {
        current++;
    }
    return tokens[current - 1];
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
