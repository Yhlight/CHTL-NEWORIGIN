#include "ParsingUtils.h"
#include "CHTLParserContext.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/PropertyNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLNode/BinaryOpNode.h"
#include "../CHTLNode/NumericLiteralNode.h"
#include <stdexcept>
#include <cctype>

namespace CHTL {

// Forward declarations for the recursive descent parser
std::shared_ptr<BaseNode> parseExpression(CHTLParserContext* context);
std::shared_ptr<BaseNode> parseTerm(CHTLParserContext* context);
std::shared_ptr<BaseNode> parseFactor(CHTLParserContext* context);
std::shared_ptr<BaseNode> parsePrimary(CHTLParserContext* context);

std::shared_ptr<BaseNode> parsePrimary(CHTLParserContext* context) {
    if (context->getCurrentToken().type == TokenType::TOKEN_NUMERIC_LITERAL) {
        std::string lexeme = context->getCurrentToken().lexeme;
        context->advance();

        std::string value_str;
        std::string unit_str;
        size_t i = 0;
        while (i < lexeme.length() && (std::isdigit(lexeme[i]) || lexeme[i] == '.')) {
            value_str += lexeme[i];
            i++;
        }
        while (i < lexeme.length()) {
            unit_str += lexeme[i];
            i++;
        }
        return std::make_shared<NumericLiteralNode>(value_str, unit_str);
    }

    if (context->getCurrentToken().type == TokenType::TOKEN_LPAREN) {
        context->advance(); // consume '('
        auto expr = parseExpression(context);
        if (context->getCurrentToken().type != TokenType::TOKEN_RPAREN) {
            throw std::runtime_error("Expected ')' after expression.");
        }
        context->advance(); // consume ')'
        return expr;
    }

    throw std::runtime_error("Unexpected token in expression: " + context->getCurrentToken().lexeme);
}

std::shared_ptr<BaseNode> parseFactor(CHTLParserContext* context) {
    auto node = parsePrimary(context);

    if (context->getCurrentToken().type == TokenType::TOKEN_POWER) {
        std::string op = context->getCurrentToken().lexeme;
        context->advance(); // consume '**'
        auto right = parseFactor(context); // Right-associativity
        node = std::make_shared<BinaryOpNode>(op, node, right);
    }

    return node;
}

std::shared_ptr<BaseNode> parseTerm(CHTLParserContext* context) {
    auto node = parseFactor(context);

    while (context->getCurrentToken().type == TokenType::TOKEN_MULTIPLY ||
           context->getCurrentToken().type == TokenType::TOKEN_DIVIDE ||
           context->getCurrentToken().type == TokenType::TOKEN_MODULO) {
        std::string op = context->getCurrentToken().lexeme;
        context->advance();
        auto right = parseFactor(context);
        node = std::make_shared<BinaryOpNode>(op, node, right);
    }

    return node;
}

std::shared_ptr<BaseNode> parseExpression(CHTLParserContext* context) {
    auto node = parseTerm(context);

    while (context->getCurrentToken().type == TokenType::TOKEN_PLUS ||
           context->getCurrentToken().type == TokenType::TOKEN_MINUS) {
        std::string op = context->getCurrentToken().lexeme;
        context->advance();
        auto right = parseTerm(context);
        node = std::make_shared<BinaryOpNode>(op, node, right);
    }

    return node;
}

void parseProperties(CHTLParserContext* context, std::shared_ptr<BaseNode> parentNode) {
    while (context->getCurrentToken().type == TokenType::TOKEN_IDENTIFIER || context->getCurrentToken().type == TokenType::TOKEN_UNQUOTED_LITERAL) {
        std::string key = context->getCurrentToken().lexeme;
        context->advance();

        if (context->getCurrentToken().type == TokenType::TOKEN_COLON || context->getCurrentToken().type == TokenType::TOKEN_ASSIGN) {
            context->advance(); // consume ':' or '='

            if (context->getCurrentToken().type == TokenType::TOKEN_IDENTIFIER && context->peek(1).type == TokenType::TOKEN_LPAREN) {
                // Template variable usage
                std::string templateName = context->getCurrentToken().lexeme;
                context->advance();
                context->advance();
                std::string variableName = context->getCurrentToken().lexeme;
                context->advance();
                if (context->getCurrentToken().type != TokenType::TOKEN_RPAREN) {
                    throw std::runtime_error("Expected ')' to close variable template usage.");
                }
                context->advance();
                auto propNode = std::make_shared<PropertyNode>(key, "");
                propNode->addChild(std::make_shared<TemplateUsageNode>(templateName, TemplateUsageType::VAR, variableName));
                parentNode->addChild(propNode);
            }
            // Check for arithmetic expression or a simple numeric literal
            else if (context->getCurrentToken().type == TokenType::TOKEN_NUMERIC_LITERAL || context->getCurrentToken().type == TokenType::TOKEN_LPAREN) {
                auto propNode = std::make_shared<PropertyNode>(key, "");
                propNode->addChild(parseExpression(context));
                parentNode->addChild(propNode);
            }
            // Regular property value (string, unquoted literal like 'red')
            else if (context->getCurrentToken().type == TokenType::TOKEN_STRING_LITERAL ||
                     context->getCurrentToken().type == TokenType::TOKEN_UNQUOTED_LITERAL ||
                     context->getCurrentToken().type == TokenType::TOKEN_IDENTIFIER) {
                std::string value = context->getCurrentToken().lexeme;
                context->advance();
                auto propNode = std::make_shared<PropertyNode>(key, value);
                parentNode->addChild(propNode);
            } else {
                break;
            }

            if (context->getCurrentToken().type == TokenType::TOKEN_SEMICOLON) {
                context->advance(); // consume ';'
            }
        } else {
            break;
        }
    }
}

std::string parse_property_value(CHTLParserContext* context) {
    std::string value;
    while (context->getCurrentToken().type != TokenType::TOKEN_COMMA &&
           context->getCurrentToken().type != TokenType::TOKEN_RBRACE &&
           !context->isAtEnd()) {
        value += context->getCurrentToken().lexeme;
        const auto& next_token = context->peek(1);
        if (next_token.type != TokenType::TOKEN_COMMA &&
            next_token.type != TokenType::TOKEN_RBRACE &&
            next_token.type != TokenType::TOKEN_EOF) {
            value += " ";
        }
        context->advance();
    }
    return value;
}

}