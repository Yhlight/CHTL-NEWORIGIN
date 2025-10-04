#include "TagState.h"
#include "DefaultState.h"
#include "TextState.h"
#include "StyleState.h"
#include "ScriptState.h"
#include "ConditionalBlockState.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/ConditionalBlockNode.h"

#include <iostream>

TagState::TagState() : expectingValue(false) {}

void TagState::handle(CHTLParser& parser, Token token) {
    switch (token.type) {
        case TokenType::IDENTIFIER:
            if (token.value == "if") {
                auto conditionalNode = std::make_unique<ConditionalBlockNode>("if");
                auto* nodePtr = conditionalNode.get();
                parser.addNode(std::move(conditionalNode));
                parser.openScope(nodePtr);
                parser.setState(std::make_unique<ConditionalBlockState>("if"));
                break;
            } else if (token.value == "else") {
                std::string conditionalType = "else";
                if (parser.peek().type == TokenType::IDENTIFIER && parser.peek().value == "if") {
                    parser.consume(); // Consume the 'if'
                    conditionalType = "else if";
                }
                auto conditionalNode = std::make_unique<ConditionalBlockNode>(conditionalType);
                auto* nodePtr = conditionalNode.get();
                parser.addNode(std::move(conditionalNode));
                parser.openScope(nodePtr);
                parser.setState(std::make_unique<ConditionalBlockState>(conditionalType));
                break;
            }

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

        case TokenType::KEYWORD_STYLE: {
            auto styleNode = std::make_unique<StyleNode>();
            auto* styleNodePtr = styleNode.get();
            parser.addNode(std::move(styleNode));
            parser.openScope(styleNodePtr);
            parser.setState(std::make_unique<StyleState>());
            break;
        }

        case TokenType::KEYWORD_SCRIPT:
            parser.setRawContentMode(true);
            parser.setState(std::make_unique<ScriptState>());
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