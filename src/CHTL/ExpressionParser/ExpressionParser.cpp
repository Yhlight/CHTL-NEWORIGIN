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

// Constructor for parsing a raw string
ExpressionParser::ExpressionParser(const std::string& input) : parser_ptr(nullptr) {
    ownedLexer = std::make_unique<CHTLLexer>(input);
}

// Constructor for parsing from an existing CHTLParser
ExpressionParser::ExpressionParser(CHTLParser& parser) : parser_ptr(&parser) {}

Token ExpressionParser::consume() {
    if (parser_ptr) {
        return parser_ptr->consume();
    }

    if (!tokenBuffer.empty()) {
        Token token = tokenBuffer.front();
        tokenBuffer.erase(tokenBuffer.begin());
        return token;
    }
    return ownedLexer->getNextToken();
}

Token ExpressionParser::peek() {
    if (parser_ptr) {
        return parser_ptr->peek();
    }

    if (tokenBuffer.empty()) {
        tokenBuffer.push_back(ownedLexer->getNextToken());
    }
    return tokenBuffer.front();
}

// Public entry point
std::unique_ptr<ExpressionNode> ExpressionParser::parse() {
    return parseExpression(0);
}

int ExpressionParser::getPrecedence() {
    return getOperatorPrecedence(peek().type);
}

std::unique_ptr<ExpressionNode> ExpressionParser::parsePrefix() {
    Token token = consume();
    if (token.type == TokenType::NUMBER || token.type == TokenType::STRING_LITERAL) {
        return std::make_unique<ValueNode>(token.value);
    } else if (token.type == TokenType::IDENTIFIER) {
        if (peek().type == TokenType::DOT) {
             // This case is not handled by the current simplified expression parser
             // and would require more context. For now, treat as a simple identifier/value.
             return std::make_unique<ValueNode>(token.value);
        }
        return std::make_unique<ReferenceNode>(token.value);
    }
    else if (token.type == TokenType::PROPERTY_REFERENCE) {
        return std::make_unique<ReferenceNode>(token.value);
    } else if (token.type == TokenType::LEFT_PAREN) {
        auto expression = parseExpression(0);
        if (consume().type != TokenType::RIGHT_PAREN) {
             throw std::runtime_error("Expected ')' in expression.");
        }
        return expression;
    }
    throw std::runtime_error("Unexpected token in expression prefix: " + token.value);
    return nullptr;
}

std::unique_ptr<ExpressionNode> ExpressionParser::parseInfix(std::unique_ptr<ExpressionNode> left) {
    Token opToken = consume(); // Consume the operator

    if (opToken.type == TokenType::QUESTION_MARK) {
        auto true_expr = parseExpression(0);
        if (consume().type != TokenType::COLON) {
            throw std::runtime_error("Expected ':' in ternary expression.");
        }
        auto false_expr = parseExpression(getOperatorPrecedence(opToken.type) - 1);
        return std::make_unique<ConditionalNode>(std::move(left), std::move(true_expr), std::move(false_expr));
    }

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