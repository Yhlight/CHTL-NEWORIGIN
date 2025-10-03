#include "ExpressionParser.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/ValueNode.h"
#include "../CHTLNode/BinaryOpNode.h"
#include "../CHTLNode/ExpressionNode.h"
#include "../CHTLNode/ReferenceNode.h"
#include "../CHTLNode/ConditionalNode.h"
#include <stdexcept>

namespace {
    // Defines the precedence of our operators.
    int getOperatorPrecedence(TokenType type) {
        switch (type) {
            case TokenType::QUESTION_MARK:    return 1;
            case TokenType::LOGICAL_OR:       return 2;
            case TokenType::LOGICAL_AND:      return 3;
            case TokenType::EQUAL_EQUAL:
            case TokenType::NOT_EQUAL:        return 4;
            case TokenType::GREATER:
            case TokenType::LESS:
            case TokenType::GREATER_EQUAL:
            case TokenType::LESS_EQUAL:       return 5;
            case TokenType::PLUS:
            case TokenType::MINUS:            return 6;
            case TokenType::STAR:
            case TokenType::SLASH:            return 7;
            case TokenType::POWER:            return 8;
            default:                          return 0;
        }
    }
}

ExpressionParser::ExpressionParser(CHTLParser& parser) : parser(parser) {}

// Public entry point
std::unique_ptr<ExpressionNode> ExpressionParser::parse() {
    return parseExpression(0);
}

int ExpressionParser::getPrecedence() {
    Token token = parser.peek();
    return getOperatorPrecedence(token.type);
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
    Token opToken = parser.consume(); // Consume the operator

    if (opToken.type == TokenType::QUESTION_MARK) {
        auto true_expr = parseExpression(0);
        if (parser.peek().type != TokenType::COLON) {
            throw std::runtime_error("Expected ':' in ternary expression.");
        }
        parser.consume(); // Consume ':'
        // Ternary operator is right-associative.
        auto false_expr = parseExpression(getOperatorPrecedence(opToken.type) - 1);
        return std::make_unique<ConditionalNode>(std::move(left), std::move(true_expr), std::move(false_expr));
    }

    // Standard binary operator logic
    int precedence = getOperatorPrecedence(opToken.type);
    auto right = parseExpression(precedence);
    return std::make_unique<BinaryOpNode>(opToken.type, std::move(left), std::move(right));
}

// The main Pratt parser logic
std::unique_ptr<ExpressionNode> ExpressionParser::parseExpression(int precedence) {
    auto left = parsePrefix();

    while (left && precedence < getPrecedence()) {
        left = parseInfix(std::move(left));
    }

    return left;
}