#include "GlobalScopeState.h"
#include "CHTLParser/Parser.h"
#include "CHTLLexer/Token.h"
#include "ParsingElementState.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h" // <-- New include
#include <iostream>

namespace CHTL {

GlobalScopeState::GlobalScopeState(Parser* parser) : parser(parser) {}

void GlobalScopeState::onUpdate(StateMachine* sm) {
    // Rule for element definition: IDENTIFIER { ... }
    if (parser->check(TokenType::IDENTIFIER) && parser->peekNext().type == TokenType::LEFT_BRACE) {
        std::string tagName = parser->advance().lexeme;
        parser->advance(); // Consume '{'

        auto elementNode = std::make_shared<ElementNode>(tagName);
        if (!parser->nodeStack.empty()) {
            parser->nodeStack.top()->addChild(elementNode);
        }
        parser->nodeStack.push(elementNode);

        sm->changeState(std::make_unique<ParsingElementState>(parser, elementNode));
        return;
    }

    // Rule for text block: text { "content" } or text { content }
    if (parser->check(TokenType::KEYWORD_TEXT) && parser->peekNext().type == TokenType::LEFT_BRACE) {
        parser->advance(); // Consume 'text'
        parser->advance(); // Consume '{'

        if (parser->check(TokenType::STRING_LITERAL) || parser->check(TokenType::IDENTIFIER)) {
            Token contentToken = parser->advance();
            std::string content = contentToken.lexeme;

            // For string literals, strip the surrounding quotes.
            if (contentToken.type == TokenType::STRING_LITERAL && content.length() >= 2) {
                content = content.substr(1, content.length() - 2);
            }

            auto textNode = std::make_shared<TextNode>(content);
            if (!parser->nodeStack.empty()) {
                parser->nodeStack.top()->addChild(textNode);
            }

            if (!parser->match({TokenType::RIGHT_BRACE})) {
                std::cerr << "Syntax Error: Missing '}' after text block on line " << contentToken.line << std::endl;
            }
            return;
        }
    }

    // If no rule matches, skip the token.
    if (!parser->isAtEnd()) {
        parser->advance();
    }
}

} // namespace CHTL
