#include "TextState.h"
#include "TagState.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/TextNode.h"
#include <iostream>

void TextState::handle(CHTLParser& parser, Token token) {
    switch (token.type) {
        case TokenType::LEFT_BRACE:
            // Expecting text content next
            break;
        case TokenType::STRING_LITERAL:
        case TokenType::IDENTIFIER:
            parser.addNode(std::make_unique<TextNode>(token.value));
            break;
        case TokenType::RIGHT_BRACE:
            // Text block is finished, go back to TagState to handle the parent element's scope.
            parser.setState(std::make_unique<TagState>());
            break;
        default:
            // Ignore other tokens for now
            break;
    }
}