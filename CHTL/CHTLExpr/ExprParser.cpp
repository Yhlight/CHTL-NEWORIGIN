#include "ExprParser.h"
#include "../CHTLParser/CHTLParser.h"
#include "LiteralNode.h"
#include "BinaryOpNode.h"
#include "TernaryOpNode.h"
#include "PropertyAccessNode.h"
#include <stdexcept>
#include "../CHTLContext/CHTLContext.h"
#include "../CHTLNode/TemplateDefinitionNode.h"

ExprParser::ExprParser(CHTLParser& chtlParser, CHTLContext& context) : parser(chtlParser), context(context) {}

std::shared_ptr<BaseExprNode> ExprParser::parse() {
    return parseTernary(); // Start with the lowest precedence
}

// Handles ternary operator: condition ? true_expr : false_expr
std::shared_ptr<BaseExprNode> ExprParser::parseTernary() {
    auto condition = parseLogicalOr();
    if (parser.getCurrentToken().type == TokenType::QUESTION_MARK) {
        parser.consumeToken(); // consume '?'
        auto trueExpr = parseExpression();
        if (parser.getCurrentToken().type != TokenType::COLON) {
             throw std::runtime_error("Expression Parser: Expected ':' for ternary operator.");
        }
        parser.consumeToken(); // consume ':'
        auto falseExpr = parseTernary(); // Right-associative
        return std::make_shared<TernaryOpNode>(condition, trueExpr, falseExpr);
    }
    return condition;
}

// Chained methods for precedence climbing
std::shared_ptr<BaseExprNode> ExprParser::parseExpression() { return parseTernary(); }

std::shared_ptr<BaseExprNode> ExprParser::parseLogicalOr() {
    auto left = parseLogicalAnd();
    while (parser.getCurrentToken().type == TokenType::LOGICAL_OR) {
        TokenType op = parser.getCurrentToken().type;
        parser.consumeToken();
        auto right = parseLogicalAnd();
        left = std::make_shared<BinaryOpNode>(left, op, right);
    }
    return left;
}

std::shared_ptr<BaseExprNode> ExprParser::parseLogicalAnd() {
    auto left = parseEquality();
    while (parser.getCurrentToken().type == TokenType::LOGICAL_AND) {
        TokenType op = parser.getCurrentToken().type;
        parser.consumeToken();
        auto right = parseEquality();
        left = std::make_shared<BinaryOpNode>(left, op, right);
    }
    return left;
}

std::shared_ptr<BaseExprNode> ExprParser::parseEquality() {
    auto left = parseComparison();
    while (parser.getCurrentToken().type == TokenType::EQUAL_EQUAL || parser.getCurrentToken().type == TokenType::NOT_EQUAL) {
        TokenType op = parser.getCurrentToken().type;
        parser.consumeToken();
        auto right = parseComparison();
        left = std::make_shared<BinaryOpNode>(left, op, right);
    }
    return left;
}

std::shared_ptr<BaseExprNode> ExprParser::parseComparison() {
    auto left = parseTerm();
    while (parser.getCurrentToken().type == TokenType::GREATER || parser.getCurrentToken().type == TokenType::LESS) {
        TokenType op = parser.getCurrentToken().type;
        parser.consumeToken();
        auto right = parseTerm();
        left = std::make_shared<BinaryOpNode>(left, op, right);
    }
    return left;
}

std::shared_ptr<BaseExprNode> ExprParser::parseTerm() {
    auto left = parseFactor();
    while (parser.getCurrentToken().type == TokenType::PLUS || parser.getCurrentToken().type == TokenType::MINUS) {
        TokenType op = parser.getCurrentToken().type;
        parser.consumeToken();
        auto right = parseFactor();
        left = std::make_shared<BinaryOpNode>(left, op, right);
    }
    return left;
}

std::shared_ptr<BaseExprNode> ExprParser::parseFactor() {
    auto left = parsePrimary();
    while (parser.getCurrentToken().type == TokenType::STAR || parser.getCurrentToken().type == TokenType::SLASH || parser.getCurrentToken().type == TokenType::PERCENT) {
        TokenType op = parser.getCurrentToken().type;
        parser.consumeToken();
        auto right = parsePrimary();
        left = std::make_shared<BinaryOpNode>(left, op, right);
    }
    return left;
}

std::shared_ptr<BaseExprNode> ExprParser::parsePrimary() {
    const Token& token = parser.getCurrentToken();

    if (token.type == TokenType::NUMBER) {
        std::string value = token.value;
        parser.consumeToken();
        if (parser.getCurrentToken().type == TokenType::IDENTIFIER) {
            value += parser.getCurrentToken().value;
            parser.consumeToken();
        }
        return std::make_shared<LiteralNode>(value);
    }

    if (token.type == TokenType::STRING) {
        auto node = std::make_shared<LiteralNode>(token.value);
        parser.consumeToken();
        return node;
    }

    if (token.type == TokenType::IDENTIFIER) {
        if (parser.peekNextToken().type == TokenType::LEFT_PAREN) {
            std::string tplName = token.value;
            parser.consumeToken();
            parser.consumeToken();
            std::string varName = parser.getCurrentToken().value;
            if(parser.getCurrentToken().type != TokenType::IDENTIFIER) throw std::runtime_error("Expected variable name.");
            parser.consumeToken();
            if(parser.getCurrentToken().type != TokenType::RIGHT_PAREN) throw std::runtime_error("Expected ')'.");
            parser.consumeToken();

            if (context.varTemplates.count(tplName) && context.varTemplates[tplName]->variables.count(varName)) {
                return std::make_shared<LiteralNode>(context.varTemplates[tplName]->variables[varName]);
            } else {
                throw std::runtime_error("Undefined variable '" + varName + "' in template '" + tplName + "'");
            }
        }
        // Otherwise, it's a property access
        auto node = std::make_shared<PropertyAccessNode>(token.value);
        parser.consumeToken();
        return node;
    }

    if (token.type == TokenType::LEFT_PAREN) {
        parser.consumeToken();
        auto expr = parseExpression();
        if (parser.getCurrentToken().type != TokenType::RIGHT_PAREN) {
            throw std::runtime_error("Expression Parser: Expected ')'");
        }
        parser.consumeToken();
        return expr;
    }

    throw std::runtime_error("Expression Parser: Unexpected token " + token.value);
    return nullptr;
}
