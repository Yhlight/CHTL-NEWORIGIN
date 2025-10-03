#include "StyleState.h"
#include "TagState.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/StylePropertyNode.h"
#include "../CHTLNode/ClassSelectorNode.h"
#include "../CHTLNode/IdSelectorNode.h"
#include "../CHTLNode/ContextSelectorNode.h"
#include "../CHTLNode/ElementNode.h"
#include <iostream>

StyleState::StyleState() : expectingValue(false), inContextualBlock(false) {}

void StyleState::handle(CHTLParser& parser, Token token) {
    // If we are inside a contextual selector's block (e.g., &:hover { ... })
    if (inContextualBlock) {
        switch (token.type) {
            case TokenType::IDENTIFIER:
                if (expectingValue) {
                    parser.addNode(std::make_unique<StylePropertyNode>(pendingPropertyName, token.value));
                    pendingPropertyName.clear();
                    expectingValue = false;
                } else {
                    pendingPropertyName = token.value;
                }
                break;
            case TokenType::COLON:
            case TokenType::EQUALS:
                expectingValue = true;
                break;
            case TokenType::SEMICOLON:
                pendingPropertyName.clear();
                expectingValue = false;
                break;
            case TokenType::RIGHT_BRACE:
                // Exiting the contextual block, so we pop its scope
                parser.closeScope();
                inContextualBlock = false;
                break;
            default:
                // Ignore other tokens inside a contextual block for now
                break;
        }
        return; // Important to return here to not fall through to the outer switch
    }

    // If we are in the main style block
    switch (token.type) {
        case TokenType::IDENTIFIER:
            if (expectingValue) {
                parser.addNode(std::make_unique<StylePropertyNode>(pendingPropertyName, token.value));
                pendingPropertyName.clear();
                expectingValue = false;
            } else {
                pendingPropertyName = token.value;
            }
            break;

        case TokenType::COLON:
        case TokenType::EQUALS:
            expectingValue = true;
            break;

        case TokenType::SEMICOLON:
            pendingPropertyName.clear();
            expectingValue = false;
            break;

        case TokenType::RIGHT_BRACE:
            // End of the main style block
            parser.closeScope();
            parser.setState(std::make_unique<TagState>());
            break;

        case TokenType::CLASS_SELECTOR:
            parser.addNode(std::make_unique<ClassSelectorNode>(token.value));
            break;

        case TokenType::ID_SELECTOR:
            parser.addNode(std::make_unique<IdSelectorNode>(token.value));
            break;

        case TokenType::CONTEXT_SELECTOR:
            pendingContextualSelector = token.value;
            break;

        case TokenType::LEFT_BRACE:
            // This brace opens the block for the pending contextual selector
            if (!pendingContextualSelector.empty()) {
                auto contextNode = std::make_unique<ContextSelectorNode>(pendingContextualSelector);
                auto* contextNodePtr = contextNode.get();
                parser.addNode(std::move(contextNode));
                parser.openScope(contextNodePtr);
                pendingContextualSelector.clear();
                inContextualBlock = true;
            }
            // If there's no pending selector, we ignore the brace for now.
            break;

        default:
            break;
    }
}