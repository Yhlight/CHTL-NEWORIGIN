#include "IfParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLNode/IfNode.h"
#include "../CHTLNode/PropertyNode.h"
#include <stdexcept>

namespace CHTL {

    // Helper to parse the value of a property until a comma or closing brace
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

                if (context->getCurrentToken().type != TokenType::TOKEN_DOT && next_token.type != TokenType::TOKEN_DOT) {
                    value += " ";
                }
            }
            context->advance();
        }
        return value;
    }

    std::shared_ptr<BaseNode> IfParsingStrategy::parse(CHTLParserContext* context) {
        auto ifNode = std::make_shared<IfNode>();
        Token currentToken = context->getCurrentToken();

        // Determine the type of the if-construct (if, else if, else)
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

        // Parse the properties inside the block
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

                std::string condition_value;
                while (context->getCurrentToken().type != TokenType::TOKEN_COMMA &&
                       context->getCurrentToken().type != TokenType::TOKEN_RBRACE &&
                       !context->isAtEnd()) {

                    if (context->getCurrentToken().type == TokenType::TOKEN_LBRACE_DOUBLE) {
                        ifNode->isDynamic = true;
                    }

                    condition_value += context->getCurrentToken().lexeme;

                    const auto& next_token = context->peek(1);
                    if (next_token.type != TokenType::TOKEN_COMMA &&
                        next_token.type != TokenType::TOKEN_RBRACE &&
                        next_token.type != TokenType::TOKEN_EOF) {

                        if (context->getCurrentToken().type != TokenType::TOKEN_LBRACE_DOUBLE &&
                            next_token.type != TokenType::TOKEN_RBRACE_DOUBLE &&
                            context->getCurrentToken().type != TokenType::TOKEN_DOT &&
                            next_token.type != TokenType::TOKEN_DOT) {
                            condition_value += " ";
                        }
                    }
                    context->advance();
                }
                ifNode->condition = condition_value;

            } else {
                std::string value = parse_property_value(context);
                ifNode->addChild(std::make_shared<PropertyNode>(key, value));
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

        // An 'if' or 'else if' must have a condition.
        if (ifNode->if_type != IfType::ELSE && ifNode->condition.empty()) {
            throw std::runtime_error("'if' and 'else if' blocks must have a 'condition' property.");
        }

        return ifNode;
    }

}