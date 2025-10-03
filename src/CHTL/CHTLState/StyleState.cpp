#include "StyleState.h"
#include "TagState.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/StylePropertyNode.h"
#include "../CHTLNode/ClassSelectorNode.h"
#include "../CHTLNode/IdSelectorNode.h"
#include "../CHTLNode/ElementNode.h"
#include <iostream>

StyleState::StyleState() : expectingValue(false) {}

void StyleState::handle(CHTLParser& parser, Token token) {
    switch (token.type) {
        case TokenType::IDENTIFIER:
            if (expectingValue) {
                // This is the property value
                parser.addNode(std::make_unique<StylePropertyNode>(pendingPropertyName, token.value));
                pendingPropertyName.clear();
                expectingValue = false;
            } else {
                // This is the property name
                pendingPropertyName = token.value;
            }
            break;

        case TokenType::COLON:
        case TokenType::EQUALS:
            expectingValue = true;
            break;

        case TokenType::SEMICOLON:
            // End of a property; reset for the next one.
            pendingPropertyName.clear();
            expectingValue = false;
            break;

        case TokenType::RIGHT_BRACE:
            // End of the style block.
            parser.closeScope();
            parser.setState(std::make_unique<TagState>());
            break;

        case TokenType::CLASS_SELECTOR:
            parser.addNode(std::make_unique<ClassSelectorNode>(token.value));
            break;

        case TokenType::ID_SELECTOR:
            parser.addNode(std::make_unique<IdSelectorNode>(token.value));
            break;

        default:
            // Ignore other tokens for now.
            break;
    }
}