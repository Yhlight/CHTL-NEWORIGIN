#include "ExprParser.h"
#include "../CHTLParser/CHTLParser.h"
#include "LiteralNode.h"
#include "BinaryOpNode.h"
#include "TernaryOpNode.h"
#include "PropertyAccessNode.h"
#include "IdentifierNode.h"
#include <stdexcept>
#include "../CHTLContext/CHTLContext.h"
#include "../CHTLNode/TemplateDefinitionNode.h"

ExprParser::ExprParser(CHTLParser& chtlParser, CHTLContext& context) : parser(chtlParser), context(context) {}

std::shared_ptr<BaseExprNode> ExprParser::parse() {
    return parseTernary(); // Start with the lowest precedence
}

// Handles ternary operator: condition ? true_expr : false_expr
// Also handles CHTL-specific chains: cond1 ? true1, cond2 ? true2 : false2
// And optional false branches: cond1 ? true1
std::shared_ptr<BaseExprNode> ExprParser::parseTernary() {
    auto condition = parseLogicalOr();
    if (parser.getCurrentToken().type != TokenType::QUESTION_MARK) {
        return condition;
    }

    parser.consumeToken(); // consume '?'
    auto trueExpr = parseLogicalOr(); // True expression cannot be a bare ternary
    std::shared_ptr<BaseExprNode> falseExpr = nullptr;

    if (parser.getCurrentToken().type == TokenType::COLON) {
        parser.consumeToken(); // consume ':'
        falseExpr = parseTernary(); // Standard right-associative ternary
    } else if (parser.getCurrentToken().type == TokenType::COMMA) {
        parser.consumeToken(); // consume ','
        falseExpr = parseTernary(); // The next item in the chain is the "else" part
    }
    // If neither, falseExpr is nullptr, representing an optional branch

    return std::make_shared<TernaryOpNode>(condition, trueExpr, falseExpr);
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

// Helper to identify tokens that act as binary operators and separate expression terms
bool isOperator(TokenType type) {
    switch(type) {
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::STAR:
        case TokenType::SLASH:
        case TokenType::PERCENT:
        case TokenType::DOUBLE_STAR:
        case TokenType::GREATER:
        case TokenType::LESS:
        case TokenType::EQUAL_EQUAL:
        case TokenType::NOT_EQUAL:
        case TokenType::LOGICAL_AND:
        case TokenType::LOGICAL_OR:
        case TokenType::QUESTION_MARK:
            return true;
        default:
            return false;
    }
}

std::shared_ptr<BaseExprNode> ExprParser::parsePrimary() {
    const Token& token = parser.getCurrentToken();

    // Handle explicit property access first, as it starts with non-identifier tokens
    if (token.type == TokenType::DOT || token.type == TokenType::HASH) {
        std::string selector_str;
        selector_str += token.value;
        parser.consumeToken(); // consume '.' or '#'

        if (parser.getCurrentToken().type != TokenType::IDENTIFIER) throw std::runtime_error("Expected selector name after '.' or '#'");
        selector_str += parser.getCurrentToken().value;
        parser.consumeToken(); // consume selector name

        if (parser.getCurrentToken().type != TokenType::DOT) throw std::runtime_error("Expected '.' before property name in selector.");
        parser.consumeToken(); // consume '.'

        if (parser.getCurrentToken().type != TokenType::IDENTIFIER) throw std::runtime_error("Expected property name after '.'");
        std::string propertyName = parser.getCurrentToken().value;
        parser.consumeToken();
        return std::make_shared<PropertyAccessNode>(selector_str, propertyName);
    }

    // Handle literals (number, string, or identifier) which can be multi-word
    if (token.type == TokenType::NUMBER || token.type == TokenType::IDENTIFIER || token.type == TokenType::STRING) {
        // Variable template usage is a special case of identifier
        if (token.type == TokenType::IDENTIFIER && parser.peekNextToken().type == TokenType::LEFT_PAREN) {
            std::string tplName = token.value;
            parser.consumeToken(); // consume IDENTIFIER
            parser.consumeToken(); // consume '('
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

        // It's a potential multi-word literal or a single identifier/property
        std::string value_str = token.value;
        parser.consumeToken();
        int word_count = 1;

        // Greedily consume subsequent identifiers/numbers if they are not followed by an operator
        while(parser.getCurrentToken().type == TokenType::IDENTIFIER || parser.getCurrentToken().type == TokenType::NUMBER) {
            value_str += " " + parser.getCurrentToken().value;
            parser.consumeToken();
            word_count++;
        }

        // If we consumed multiple words, it must be a literal.
        if (word_count > 1) {
            return std::make_shared<LiteralNode>(value_str);
        } else {
            // It was a single word. Check the original token type.
            if (token.type == TokenType::NUMBER) {
                return std::make_shared<LiteralNode>(value_str);
            }

            // If it was an IDENTIFIER, it could be `div.width` or just `width`
            if (parser.getCurrentToken().type == TokenType::DOT) {
                std::string selector_str = value_str;
                parser.consumeToken(); // consume '.'
                if (parser.getCurrentToken().type != TokenType::IDENTIFIER) throw std::runtime_error("Expected property name after '.'");
                std::string propertyName = parser.getCurrentToken().value;
                parser.consumeToken();
                return std::make_shared<PropertyAccessNode>(selector_str, propertyName);
            }
            // Otherwise, it's an ambiguous identifier.
            // PATCH: If the identifier is purely numeric, treat it as a literal.
            // This handles the case where the parser logic incorrectly identifies a number
            // as an identifier.
            bool is_numeric_string = true;
            for (char c : value_str) {
                if (!isdigit(c) && c != '.') {
                    is_numeric_string = false;
                    break;
                }
            }
            if (is_numeric_string) {
                return std::make_shared<LiteralNode>(value_str);
            }
            return std::make_shared<IdentifierNode>(value_str);
        }
    }

    // Handle Parenthesized expression
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
}
