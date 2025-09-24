#include "ParseElementState.h"
#include "ParseBaseState.h"
#include "ParseStyleState.h"
#include "CHTLContext/Context.h"
#include "CHTLNode/AttributeNode.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/StyleBlockNode.h"
#include <iostream>
#include <stdexcept>

namespace CHTL {

void ParseElementState::handle(CHTLContext& context) {
    while (context.peekToken().type != TokenType::RIGHT_BRACE &&
           context.peekToken().type != TokenType::END_OF_FILE) {

        Token currentToken = context.peekToken();

        if (currentToken.type == TokenType::IDENTIFIER) {
            Token lookahead = context.peekToken(1);

            if (lookahead.type == TokenType::COLON) {
                // Attribute Parsing
                std::string key = context.consumeToken().lexeme;
                context.consumeToken(); // Consume ':'
                Token valueToken = context.consumeToken();
                if (valueToken.type != TokenType::STRING && valueToken.type != TokenType::IDENTIFIER && valueToken.type != TokenType::UNQUOTED_LITERAL) {
                    throw std::runtime_error("Expected attribute value.");
                }
                if (context.consumeToken().type != TokenType::SEMICOLON) {
                    throw std::runtime_error("Expected ';' after attribute value.");
                }
                auto attrNode = std::make_unique<AttributeNode>(key, valueToken.lexeme);
                context.getCurrentNode()->attributes.push_back(std::move(attrNode));

            } else if (lookahead.type == TokenType::LEFT_BRACE) {
                // Nested Element Parsing
                std::string tagName = context.consumeToken().lexeme;
                context.consumeToken(); // Consume '{'
                auto newElement = std::make_unique<ElementNode>(tagName);
                ElementNode* newElementPtr = newElement.get();
                context.getCurrentNode()->children.push_back(std::move(newElement));
                context.pushNode(newElementPtr);
                context.setState(std::make_unique<ParseElementState>());
                return;

            } else {
                 throw std::runtime_error("Unexpected token after identifier in element: " + lookahead.lexeme);
            }
        } else if (currentToken.type == TokenType::KEYWORD_TEXT) {
            // Text Node Parsing
            context.consumeToken(); // Consume 'text'
            if (context.consumeToken().type != TokenType::COLON) {
                throw std::runtime_error("Expected ':' after 'text' keyword.");
            }
            Token valueToken = context.consumeToken();
            if (valueToken.type != TokenType::STRING) {
                throw std::runtime_error("Expected string literal for text node.");
            }
            if (context.consumeToken().type != TokenType::SEMICOLON) {
                throw std::runtime_error("Expected ';' after text node value.");
            }
            auto textNode = std::make_unique<TextNode>(valueToken.lexeme);
            context.getCurrentNode()->children.push_back(std::move(textNode));
        } else if (currentToken.type == TokenType::KEYWORD_STYLE) {
            // Style Block Parsing
            context.consumeToken(); // Consume 'style'
            if (context.consumeToken().type != TokenType::LEFT_BRACE) {
                throw std::runtime_error("Expected '{' after 'style' keyword.");
            }
            auto styleNode = std::make_unique<StyleBlockNode>();
            // Set the current style block on the context so the strategy can access it
            context.setCurrentStyleBlock(styleNode.get());
            context.getCurrentNode()->style = std::move(styleNode);

            context.setState(std::make_unique<ParseStyleState>());
            return;
        }
        else {
             throw std::runtime_error("Unexpected token in element: " + currentToken.lexeme);
        }
    }

    if (context.peekToken().type == TokenType::RIGHT_BRACE) {
        context.consumeToken();
    } else {
        throw std::runtime_error("Expected '}' to close element block.");
    }

    context.popNode();

    if (context.getCurrentNode() == nullptr) {
        context.setState(std::make_unique<ParseBaseState>());
    } else {
        context.setState(std::make_unique<ParseElementState>());
    }
}

} // namespace CHTL
