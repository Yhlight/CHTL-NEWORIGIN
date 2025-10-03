#include "DeclarationState.h"
#include "DefaultState.h"
#include "TagState.h"
#include "StyleState.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include "../CHTLNode/CustomDefinitionNode.h"

#include <iostream>

DeclarationState::DeclarationState() : phase(Phase::AwaitingKeyword) {}

void DeclarationState::handle(CHTLParser &parser, Token token) {
    switch (phase) {
    case Phase::AwaitingKeyword:
        if (token.type == TokenType::KEYWORD_TEMPLATE) {
            declarationType = "Template";
            phase = Phase::AwaitingRightBracket;
        } else if (token.type == TokenType::KEYWORD_CUSTOM) {
            declarationType = "Custom";
            phase = Phase::AwaitingRightBracket;
        }
        break;

    case Phase::AwaitingRightBracket:
        if (token.type == TokenType::RIGHT_BRACKET) {
            phase = Phase::AwaitingAt;
        }
        break;

    case Phase::AwaitingAt:
        if (token.type == TokenType::AT_SIGN) {
            phase = Phase::AwaitingType;
        }
        break;

    case Phase::AwaitingType:
        if (token.type == TokenType::IDENTIFIER || token.type == TokenType::KEYWORD_STYLE || token.type == TokenType::KEYWORD_ELEMENT) {
            blockType = token.value;
            phase = Phase::AwaitingName;
        }
        break;

    case Phase::AwaitingName:
        if (token.type == TokenType::IDENTIFIER) {
            blockName = token.value;
            phase = Phase::AwaitingLeftBrace;
        }
        break;

    case Phase::AwaitingLeftBrace:
        if (token.type == TokenType::LEFT_BRACE) {
            BaseNode* defNodePtr = nullptr;
            if (declarationType == "Template") {
                auto defNode = std::make_unique<TemplateDefinitionNode>(blockType, blockName);
                defNodePtr = defNode.get();
                parser.addNode(std::move(defNode));
            } else { // Custom
                auto defNode = std::make_unique<CustomDefinitionNode>(blockType, blockName);
                defNodePtr = defNode.get();
                parser.addNode(std::move(defNode));
            }
            parser.openScope(defNodePtr);

            if (blockType == "Style" || blockType == "style") {
                parser.setState(std::make_unique<StyleState>());
            } else {
                parser.setState(std::make_unique<TagState>());
            }
        }
        break;
    }
}