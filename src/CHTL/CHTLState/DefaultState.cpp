#include "DefaultState.h"
#include "TagState.h"
#include "TextState.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/CommentNode.h"

#include <iostream>

void DefaultState::handle(CHTLParser& parser, Token token) {
    switch (token.type) {
        case TokenType::IDENTIFIER:
            pendingTagName = token.value;
            break;

        case TokenType::LEFT_BRACE:
            if (!pendingTagName.empty()) {
                auto elementNode = std::make_unique<ElementNode>(pendingTagName);
                auto* elementPtr = elementNode.get();
                parser.addNode(std::move(elementNode));
                parser.openScope(elementPtr);
                pendingTagName.clear(); // Clear the name BEFORE changing the state
                parser.setState(std::make_unique<TagState>());
            }
            break;

        case TokenType::KEYWORD_TEXT:
            parser.setState(std::make_unique<TextState>());
            break;

        case TokenType::SINGLE_LINE_COMMENT:
        case TokenType::MULTI_LINE_COMMENT:
        case TokenType::GENERATOR_COMMENT:
            parser.addNode(std::make_unique<CommentNode>(token.value));
            break;

        default:
            // Ignore other tokens in the default state for now
            break;
    }
}