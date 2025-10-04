#include "IfParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
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
            throw std::runtime_error("Expected '{' after if/else if/else.");
        }
        context->advance(); // consume '{'

        while (context->getCurrentToken().type != TokenType::TOKEN_RBRACE && !context->isAtEnd()) {
            if (context->getCurrentToken().type != TokenType::TOKEN_IDENTIFIER && context->getCurrentToken().type != TokenType::TOKEN_UNQUOTED_LITERAL) {
                throw std::runtime_error("Expected property identifier or unquoted literal in if block.");
            }
            std::string key = context->getCurrentToken().lexeme;
            context->advance();

            if (context->getCurrentToken().type != TokenType::TOKEN_COLON) {
                throw std::runtime_error("Expected ':' after property key.");
            }
            context->advance(); // consume ':'

            std::string value;
            while (context->getCurrentToken().type != TokenType::TOKEN_SEMICOLON && context->getCurrentToken().type != TokenType::TOKEN_RBRACE && !context->isAtEnd()) {
                value += context->getCurrentToken().lexeme + " ";
                context->advance();
            }
             if (!value.empty()) {
                value.pop_back(); // Trim trailing space
            }

            if (key == "condition") {
                ifNode->condition = value;
            } else {
                ifNode->addChild(std::make_shared<PropertyNode>(key, value));
            }

            if (context->getCurrentToken().type == TokenType::TOKEN_SEMICOLON) {
                context->advance(); // consume ';'
            }
        }

        if (context->getCurrentToken().type != TokenType::TOKEN_RBRACE) {
            throw std::runtime_error("Expected '}' to close if block.");
        }
        context->advance(); // consume '}'

        return ifNode;
    }

}