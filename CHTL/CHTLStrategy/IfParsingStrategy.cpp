#include "IfParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLParser/ParsingUtils.h"
#include "../CHTLNode/IfNode.h"
#include "../CHTLNode/PropertyNode.h"
#include <stdexcept>

namespace CHTL {
    std::shared_ptr<BaseNode> IfParsingStrategy::parse(CHTLParserContext* context) {
        auto ifNode = std::make_shared<IfNode>();
        Token currentToken = context->getCurrentToken();

        if (currentToken.type == TokenType::TOKEN_IF) {
            ifNode->if_type = IfType::IF;
            context->advance(); // consume 'if'
        } else if (currentToken.type == TokenType::TOKEN_ELSE) {
            context->advance(); // consume 'else'
            if (context->getCurrentToken().type == TokenType::TOKEN_IF) {
                ifNode->if_type = IfType::ELSE_IF;
                context->advance(); // consume 'if'
            } else {
                ifNode->if_type = IfType::ELSE;
            }
        }

        if (context->getCurrentToken().type != TokenType::TOKEN_LBRACE) {
            throw std::runtime_error("Expected '{' after if/else if/else construct.");
        }
        context->advance(); // consume '{'

        while (context->getCurrentToken().type != TokenType::TOKEN_RBRACE && !context->isAtEnd()) {
            if (context->getCurrentToken().type != TokenType::TOKEN_IDENTIFIER) {
                throw std::runtime_error("Expected property identifier in if block.");
            }
            std::string key = context->getCurrentToken().lexeme;
            context->advance();

            if (context->getCurrentToken().type != TokenType::TOKEN_COLON) {
                throw std::runtime_error("Expected ':' after property key '" + key + "'.");
            }
            context->advance(); // consume ':'

            if (key == "condition") {
                if (ifNode->if_type == IfType::ELSE) {
                    throw std::runtime_error("'else' block cannot have a condition.");
                }
                ifNode->condition = parse_property_value(context);
            } else {
                auto propNode = std::make_shared<PropertyNode>(key, "");
                if (context->getCurrentToken().type == TokenType::TOKEN_NUMERIC_LITERAL || context->getCurrentToken().type == TokenType::TOKEN_LPAREN) {
                    propNode->addChild(parseExpression(context));
                } else {
                    propNode->setValue(parse_property_value(context));
                }
                ifNode->addChild(propNode);
            }

            if (context->getCurrentToken().type == TokenType::TOKEN_COMMA) {
                context->advance(); // consume ','
            } else if (context->getCurrentToken().type != TokenType::TOKEN_RBRACE) {
                throw std::runtime_error("Expected ',' or '}' after property in if block.");
            }
        }

        if (context->getCurrentToken().type != TokenType::TOKEN_RBRACE) {
            throw std::runtime_error("Expected '}' to close if block.");
        }
        context->advance(); // consume '}'

        if ((ifNode->if_type == IfType::IF || ifNode->if_type == IfType::ELSE_IF) && ifNode->condition.empty()) {
            throw std::runtime_error("'if' and 'else if' blocks must have a 'condition' property.");
        }

        return ifNode;
    }

}