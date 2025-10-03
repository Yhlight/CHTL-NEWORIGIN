#include "StyleState.h"
#include "TagState.h"
#include "../CHTLParser/CHTLParser.h"
#include "../ExpressionParser/ExpressionParser.h"
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
        if (token.type == TokenType::RIGHT_BRACE) {
            parser.closeScope();
            inContextualBlock = false;
        } else {
            // Inside a contextual block, we expect property: value pairs
             if (token.type == TokenType::IDENTIFIER && !expectingValue) {
                pendingPropertyName = token.value;
            } else if (token.type == TokenType::COLON || token.type == TokenType::EQUALS) {
                expectingValue = true;
            } else if (expectingValue) {
                parser.putback(token);
                ExpressionParser exprParser(parser);
                auto valueNode = exprParser.parse();
                parser.addNode(std::make_unique<StylePropertyNode>(pendingPropertyName, std::move(valueNode)));
                pendingPropertyName.clear();
                expectingValue = false;
            }
        }
        return;
    }

    // If we are in the main style block
    switch (token.type) {
        case TokenType::IDENTIFIER:
             if (expectingValue) { // This case handles unquoted literals like 'red'
                parser.putback(token);
                ExpressionParser exprParser(parser);
                auto valueNode = exprParser.parse();
                parser.addNode(std::make_unique<StylePropertyNode>(pendingPropertyName, std::move(valueNode)));
                pendingPropertyName.clear();
                expectingValue = false;
            } else {
                pendingPropertyName = token.value;
            }
            break;

        case TokenType::NUMBER:
        case TokenType::STRING_LITERAL:
        case TokenType::LEFT_PAREN: // Start of a grouped expression
            if (expectingValue) {
                parser.putback(token);
                ExpressionParser exprParser(parser);
                auto valueNode = exprParser.parse();
                parser.addNode(std::make_unique<StylePropertyNode>(pendingPropertyName, std::move(valueNode)));
                pendingPropertyName.clear();
                expectingValue = false;
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
            break;

        default:
            break;
    }
}