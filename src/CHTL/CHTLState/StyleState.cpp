#include "StyleState.h"
#include "TagState.h"
#include "../CHTLParser/CHTLParser.h"
#include "../ExpressionParser/ExpressionParser.h"
#include "../CHTLNode/StylePropertyNode.h"
#include "../CHTLNode/ClassSelectorNode.h"
#include "../CHTLNode/IdSelectorNode.h"
#include "../CHTLNode/ContextSelectorNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/ValueNode.h"
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
                Token nextToken = parser.peek();
                if ((token.type == TokenType::IDENTIFIER || token.type == TokenType::NUMBER || token.type == TokenType::STRING_LITERAL) &&
                    (nextToken.type == TokenType::SEMICOLON || nextToken.type == TokenType::RIGHT_BRACE))
                {
                    auto valueNode = std::make_unique<ValueNode>(token.value);
                    parser.addNode(std::make_unique<StylePropertyNode>(pendingPropertyName, std::move(valueNode)));
                } else {
                    parser.putback(token);
                    ExpressionParser exprParser(parser);
                    auto valueNode = exprParser.parse();
                    parser.addNode(std::make_unique<StylePropertyNode>(pendingPropertyName, std::move(valueNode)));
                }
                pendingPropertyName.clear();
                expectingValue = false;
            }
        }
        return;
    }

    // If we are in the main style block
    switch (token.type) {
        case TokenType::IDENTIFIER:
        case TokenType::NUMBER:
        case TokenType::STRING_LITERAL:
        case TokenType::LEFT_PAREN:
        case TokenType::PROPERTY_REFERENCE:
            if (expectingValue) {
                Token nextToken = parser.peek();
                if ((token.type == TokenType::IDENTIFIER || token.type == TokenType::NUMBER || token.type == TokenType::STRING_LITERAL) &&
                    (nextToken.type == TokenType::SEMICOLON || nextToken.type == TokenType::RIGHT_BRACE))
                {
                    auto valueNode = std::make_unique<ValueNode>(token.value);
                    parser.addNode(std::make_unique<StylePropertyNode>(pendingPropertyName, std::move(valueNode)));
                } else {
                    parser.putback(token);
                    ExpressionParser exprParser(parser);
                    auto valueNode = exprParser.parse();
                    parser.addNode(std::make_unique<StylePropertyNode>(pendingPropertyName, std::move(valueNode)));
                }
                pendingPropertyName.clear();
                expectingValue = false;
            } else if (token.type == TokenType::IDENTIFIER) {
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