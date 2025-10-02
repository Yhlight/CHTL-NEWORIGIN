#include "TagState.h"
#include "DefaultState.h"
#include "TextState.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/CommentNode.h"

#include <iostream>

TagState::TagState() : expectingValue(false) {}

void TagState::handle(CHTLParser& parser, Token token) {
    switch (token.type) {
        case TokenType::IDENTIFIER:
            if (expectingValue) {
                BaseNode* currentScope = parser.getCurrentScope();
                if (currentScope && currentScope->getType() == NodeType::Element) {
                    static_cast<ElementNode*>(currentScope)->setAttribute(pendingAttributeName, token.value);
                }
                pendingAttributeName.clear();
                expectingValue = false;
            } else {
                pendingAttributeName = token.value;
            }
            break;

        case TokenType::COLON:
        case TokenType::EQUALS:
            expectingValue = true;
            break;

        case TokenType::STRING_LITERAL:
            if (expectingValue) {
                BaseNode* currentScope = parser.getCurrentScope();
                if (currentScope && currentScope->getType() == NodeType::Element) {
                    static_cast<ElementNode*>(currentScope)->setAttribute(pendingAttributeName, token.value);
                }
                pendingAttributeName.clear();
                expectingValue = false;
            }
            break;

        case TokenType::LEFT_BRACE:
            if (!pendingAttributeName.empty()) {
                auto elementNode = std::make_unique<ElementNode>(pendingAttributeName);
                auto* elementPtr = elementNode.get();
                parser.addNode(std::move(elementNode));
                parser.openScope(elementPtr);
                pendingAttributeName.clear();
            }
            break;

        case TokenType::RIGHT_BRACE:
            parser.closeScope();
            if (parser.getCurrentScope() == parser.getRoot()) {
                parser.setState(std::make_unique<DefaultState>());
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
            // Ignore other tokens for now
            break;
    }
}