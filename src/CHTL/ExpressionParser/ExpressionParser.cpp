#include "ExpressionParser.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/ValueNode.h"
#include "../CHTLNode/BinaryOpNode.h"
#include "../CHTLNode/ExpressionNode.h"
#include "../CHTLNode/ReferenceNode.h"

ExpressionParser::ExpressionParser(CHTLParser& parser) : parser(parser) {}

// Public entry point
std::unique_ptr<ExpressionNode> ExpressionParser::parse() {
    return parseExpression(0);
}

int ExpressionParser::getPrecedence() {
    Token token = parser.peek();
    switch (token.type) {
        case TokenType::PLUS:
        case TokenType::MINUS:
            return 1;
        case TokenType::STAR:
        case TokenType::SLASH:
            return 2;
        case TokenType::POWER:
            return 3;
        default:
            return 0;
    }
}

std::unique_ptr<ExpressionNode> ExpressionParser::parsePrefix() {
    Token token = parser.consume();
    if (token.type == TokenType::NUMBER || token.type == TokenType::IDENTIFIER || token.type == TokenType::STRING_LITERAL) {
        return std::make_unique<ValueNode>(token.value);
    } else if (token.type == TokenType::PROPERTY_REFERENCE) {
        return std::make_unique<ReferenceNode>(token.value);
    } else if (token.type == TokenType::LEFT_PAREN) {
        auto expression = parseExpression(0);
        parser.consume(); // Consume ')'
        return expression;
    }
    return nullptr;
}

std::unique_ptr<ExpressionNode> ExpressionParser::parseInfix(std::unique_ptr<ExpressionNode> left) {
    Token opToken = parser.consume();

    int precedence = getPrecedence();

    auto right = parseExpression(precedence);
    return std::make_unique<BinaryOpNode>(opToken.type, std::move(left), std::move(right));
}

// The main Pratt parser logic
std::unique_ptr<ExpressionNode> ExpressionParser::parseExpression(int precedence) {
    auto left = parsePrefix();

    while (precedence < getPrecedence()) {
        left = parseInfix(std::move(left));
    }

    return left;
}