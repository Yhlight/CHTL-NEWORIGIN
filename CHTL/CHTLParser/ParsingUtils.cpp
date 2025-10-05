#include "ParsingUtils.h"
#include "CHTLParserContext.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/PropertyNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLNode/BinaryOpNode.h"
#include "../CHTLNode/NumericLiteralNode.h"
#include "../CHTLNode/ReferenceNode.h"
#include "../CHTLNode/BooleanLiteralNode.h"
#include <stdexcept>
#include <cctype>

namespace CHTL {

// Forward declarations for the recursive descent parser
std::shared_ptr<BaseNode> parseExpression(CHTLParserContext* context);
std::shared_ptr<BaseNode> parseTerm(CHTLParserContext* context);
std::shared_ptr<BaseNode> parseFactor(CHTLParserContext* context);
std::shared_ptr<BaseNode> parsePrimary(CHTLParserContext* context);

std::shared_ptr<BaseNode> parsePrimary(CHTLParserContext* context) {
    Token currentToken = context->getCurrentToken();

    if (currentToken.lexeme == "true") {
        context->advance();
        return std::make_shared<BooleanLiteralNode>(true);
    }
    if (currentToken.lexeme == "false") {
        context->advance();
        return std::make_shared<BooleanLiteralNode>(false);
    }

    if (currentToken.type == TokenType::TOKEN_NUMERIC_LITERAL) {
        std::string lexeme = currentToken.lexeme;
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

    if (currentToken.type == TokenType::TOKEN_LPAREN) {
        context->advance(); // consume '('
        auto expr = parseExpression(context);
        if (context->getCurrentToken().type != TokenType::TOKEN_RPAREN) {
            throw std::runtime_error("Expected ')' after expression.");
        }
        context->advance(); // consume ')'
        return expr;
    }

    if (currentToken.type == TokenType::TOKEN_IDENTIFIER || currentToken.type == TokenType::TOKEN_DOT) {
        std::string selector;
        if (currentToken.type == TokenType::TOKEN_DOT) {
            selector += ".";
            context->advance();
            selector += context->getCurrentToken().lexeme;
            context->advance();
        } else {
            selector = currentToken.lexeme;
            context->advance();
        }

        if (context->getCurrentToken().type == TokenType::TOKEN_DOT) {
            context->advance(); // consume '.'
            std::string propName = context->getCurrentToken().lexeme;
            context->advance();
            return std::make_shared<ReferenceNode>(selector, propName);
        }
    }

    throw std::runtime_error("Unexpected token in expression: " + currentToken.lexeme);
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

ElementTarget parseElementTarget(CHTLParserContext* context) {
    ElementTarget target;
    if (context->getCurrentToken().type != TokenType::TOKEN_IDENTIFIER) {
        throw std::runtime_error("Expected an element tag name.");
    }
    target.tagName = context->getCurrentToken().lexeme;
    context->advance();

    if (context->getCurrentToken().type == TokenType::TOKEN_LBRACKET) {
        context->advance(); // consume '['
        if (context->getCurrentToken().type != TokenType::TOKEN_NUMERIC_LITERAL) {
            throw std::runtime_error("Expected a numeric index inside [].");
        }
        target.index = std::stoi(context->getCurrentToken().lexeme);
        context->advance(); // consume numeric literal
        if (context->getCurrentToken().type != TokenType::TOKEN_RBRACKET) {
            throw std::runtime_error("Expected ']' to close index.");
        }
        context->advance(); // consume ']'
    }
    return target;
}

}