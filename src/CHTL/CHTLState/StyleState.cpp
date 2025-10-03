#include "StyleState.h"
#include "TagState.h"
#include "DefaultState.h"
#include "../CHTLParser/CHTLParser.h"
#include "../ExpressionParser/ExpressionParser.h"
#include "../CHTLNode/StylePropertyNode.h"
#include "../CHTLNode/ClassSelectorNode.h"
#include "../CHTLNode/IdSelectorNode.h"
#include "../CHTLNode/ContextSelectorNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include <iostream>

StyleState::StyleState() : expectingValue(false), inContextualBlock(false) {}

void StyleState::handle(CHTLParser& parser, Token token) {
    if (inContextualBlock) {
        if (token.type == TokenType::RIGHT_BRACE) {
            parser.closeScope();
            inContextualBlock = false;
        } else {
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

    switch (token.type) {
        case TokenType::IDENTIFIER:
             if (expectingValue) {
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
        case TokenType::LEFT_PAREN:
        case TokenType::PROPERTY_REFERENCE:
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

        case TokenType::RIGHT_BRACE: {
            parser.closeScope(); // Close the StyleNode or ContextSelectorNode
            BaseNode* parent = parser.getCurrentScope();
            if (parent && (parent->getType() == NodeType::TemplateDefinition || parent->getType() == NodeType::CustomDefinition)) {
                parser.closeScope(); // Close the Template/Custom Definition Node
                parser.setState(std::make_unique<DefaultState>());
            } else {
                parser.setState(std::make_unique<TagState>());
            }
            break;
        }

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

        case TokenType::AT_SIGN: {
            Token typeToken = parser.consume();
            Token nameToken = parser.consume();
            if ((typeToken.type == TokenType::IDENTIFIER || typeToken.type == TokenType::KEYWORD_STYLE) && nameToken.type == TokenType::IDENTIFIER) {
                parser.addNode(std::make_unique<TemplateUsageNode>(typeToken.value, nameToken.value));
            }
            break;
        }

        default:
            break;
    }
}