#include "ParsingElementState.h"
#include "CHTLParser/Parser.h"
#include "GlobalScopeState.h"
#include "CHTLLexer/Token.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include <utility>
#include <iostream>

namespace CHTL {

ParsingElementState::ParsingElementState(Parser* parser, std::shared_ptr<ElementNode> currentNode)
    : parser(parser), currentNode(std::move(currentNode)) {}

void ParsingElementState::onUpdate(StateMachine* sm) {
    // Rule 1: Nested element definition
    if (parser->check(TokenType::IDENTIFIER) && parser->peekNext().type == TokenType::LEFT_BRACE) {
        std::string tagName = parser->advance().lexeme;
        parser->advance(); // Consume '{'

        auto elementNode = std::make_shared<ElementNode>(tagName);
        currentNode->addChild(elementNode);
        parser->nodeStack.push(elementNode);

        sm->changeState(std::make_unique<ParsingElementState>(parser, elementNode));
        return;
    }

    // Rule 2: Nested text block
    if (parser->check(TokenType::KEYWORD_TEXT) && parser->peekNext().type == TokenType::LEFT_BRACE) {
        parser->advance();
        parser->advance();

        if (parser->check(TokenType::STRING_LITERAL) || parser->check(TokenType::IDENTIFIER)) {
            Token contentToken = parser->advance();
            std::string content = contentToken.lexeme;
            if (contentToken.type == TokenType::STRING_LITERAL && content.length() >= 2) {
                content = content.substr(1, content.length() - 2);
            }
            auto textNode = std::make_shared<TextNode>(content);
            currentNode->addChild(textNode);

            if (!parser->match({TokenType::RIGHT_BRACE})) {
                std::cerr << "Syntax Error: Missing '}' after text block on line " << contentToken.line << std::endl;
            }
        }
        return;
    }

    // Rule 3: Attribute or 'text' property. The key can be an identifier or a keyword like 'text'.
    if ((parser->check(TokenType::IDENTIFIER) || parser->check(TokenType::KEYWORD_TEXT)) &&
        (parser->peekNext().type == TokenType::COLON || parser->peekNext().type == TokenType::EQUAL)) {

        Token key = parser->advance();
        parser->advance();

        if (parser->check(TokenType::IDENTIFIER) || parser->check(TokenType::STRING_LITERAL) || parser->check(TokenType::NUMBER_LITERAL)) {
            Token valueToken = parser->advance();
            if (key.lexeme == "text") {
                std::string content = valueToken.lexeme;
                if (valueToken.type == TokenType::STRING_LITERAL && content.length() >= 2) {
                    content = content.substr(1, content.length() - 2);
                }
                auto textNode = std::make_shared<TextNode>(content);
                currentNode->addChild(textNode);
            } else {
                currentNode->attributes[key.lexeme] = valueToken.lexeme;
            }
            if (!parser->match({TokenType::SEMICOLON})) {
                std::cerr << "Syntax Error: Missing semicolon after attribute value on line " << valueToken.line << std::endl;
            }
            return;
        }
    }

    // Rule 4: Closing brace
    if (parser->match({TokenType::RIGHT_BRACE})) {
        parser->nodeStack.pop();

        if (parser->nodeStack.empty()) {
            sm->changeState(std::make_unique<GlobalScopeState>(parser));
        } else {
            auto parentNode = std::dynamic_pointer_cast<ElementNode>(parser->nodeStack.top());
            if (parentNode && parentNode->tagName != "__ROOT__") {
                sm->changeState(std::make_unique<ParsingElementState>(parser, parentNode));
            } else {
                sm->changeState(std::make_unique<GlobalScopeState>(parser));
            }
        }
        return;
    }

    // If no rule matches, advance
    if (!parser->isAtEnd()) {
        parser->advance();
    }
}

} // namespace CHTL
