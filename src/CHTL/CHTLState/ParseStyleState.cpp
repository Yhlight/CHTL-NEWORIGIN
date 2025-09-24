#include "ParseStyleState.h"
#include "ParseElementState.h"
#include "CHTLContext/Context.h"
#include "CHTLStrategy/ParseSimplePropertyStrategy.h"
#include "CHTLStrategy/ParseSelectorStrategy.h"
#include <iostream>
#include <stdexcept>

namespace CHTL {

void ParseStyleState::handle(CHTLContext& context) {
    while (context.peekToken().type != TokenType::RIGHT_BRACE &&
           context.peekToken().type != TokenType::END_OF_FILE) {

        Token currentToken = context.peekToken();

        if (currentToken.type == TokenType::IDENTIFIER) {
            Token lookahead = context.peekToken(1);
            if (lookahead.type == TokenType::COLON) {
                context.setStrategy(std::make_unique<ParseSimplePropertyStrategy>());
            } else {
                throw std::runtime_error("Unexpected token after identifier in style block: " + lookahead.lexeme);
            }
        } else if (currentToken.type == TokenType::DOT ||
                   currentToken.type == TokenType::HASH ||
                   currentToken.type == TokenType::AMPERSAND) {
            context.setStrategy(std::make_unique<ParseSelectorStrategy>());
        } else {
            throw std::runtime_error("Unexpected token in style block: " + currentToken.lexeme);
        }

        context.executeStrategy();
    }

    if (context.peekToken().type != TokenType::RIGHT_BRACE) {
        throw std::runtime_error("Expected '}' to close style block.");
    }
    context.consumeToken(); // Consume the '}'

    // We are done with this style block, so pop it from the container stack
    context.popStyleContainer();

    context.setState(std::make_unique<ParseElementState>());
}

} // namespace CHTL
