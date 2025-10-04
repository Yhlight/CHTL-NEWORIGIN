#include "ElementParsingStrategy.h"
#include "TextParsingStrategy.h"
#include "StyleParsingStrategy.h"
#include "IfParsingStrategy.h"
#include "ConstraintParsingStrategy.h"
#include "SpecializationParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include <stdexcept>

namespace CHTL {

void parseAttributes(CHTLParserContext* context, std::shared_ptr<ElementNode> element) {
    while (context->getCurrentToken().type == TokenType::TOKEN_IDENTIFIER) {
        if (context->peek(1).type == TokenType::TOKEN_COLON || context->peek(1).type == TokenType::TOKEN_ASSIGN) {
            std::string key = context->getCurrentToken().lexeme;
            context->advance(); // consume identifier
            context->advance(); // consume ':' or '='
            if (context->getCurrentToken().type == TokenType::TOKEN_STRING_LITERAL || context->getCurrentToken().type == TokenType::TOKEN_UNQUOTED_LITERAL || context->getCurrentToken().type == TokenType::TOKEN_NUMERIC_LITERAL || context->getCurrentToken().type == TokenType::TOKEN_IDENTIFIER) {
                std::string value = context->getCurrentToken().lexeme;
                context->advance(); // consume value
                element->setAttribute(key, value);
                if (context->getCurrentToken().type == TokenType::TOKEN_SEMICOLON) {
                    context->advance(); // consume ';'
                }
            } else {
                break;
            }
        } else {
            break;
        }
    }
}


std::shared_ptr<BaseNode> ElementParsingStrategy::parse(CHTLParserContext* context) {
    Token identifier = context->getCurrentToken();
    context->advance();
    auto element = std::make_shared<ElementNode>(identifier.lexeme);

    auto previousElement = context->getCurrentElement();
    context->setCurrentElement(element);

    if (context->getCurrentToken().type == TokenType::TOKEN_LBRACE) {
        context->advance(); // Consume '{'

        while (context->getCurrentToken().type != TokenType::TOKEN_RBRACE && !context->isAtEnd()) {
            TokenType currentType = context->getCurrentToken().type;

            if (currentType == TokenType::TOKEN_STYLE) {
                context->setStrategy(std::make_unique<StyleParsingStrategy>());
                element->addChild(context->runCurrentStrategy());
            } else if (currentType == TokenType::TOKEN_TEXT) {
                context->setStrategy(std::make_unique<TextParsingStrategy>());
                element->addChild(context->runCurrentStrategy());
            } else if (currentType == TokenType::TOKEN_IF || currentType == TokenType::TOKEN_ELSE) {
                context->setStrategy(std::make_unique<IfParsingStrategy>());
                element->addChild(context->runCurrentStrategy());
            } else if (currentType == TokenType::TOKEN_EXCEPT) {
                context->setStrategy(std::make_unique<ConstraintParsingStrategy>());
                element->addChild(context->runCurrentStrategy());
            } else if (currentType == TokenType::TOKEN_IDENTIFIER) {
                if (context->peek(1).type == TokenType::TOKEN_COLON || context->peek(1).type == TokenType::TOKEN_ASSIGN) {
                    parseAttributes(context, element);
                } else {
                    context->setStrategy(std::make_unique<ElementParsingStrategy>());
                    element->addChild(context->runCurrentStrategy());
                }
            } else if (currentType == TokenType::TOKEN_AT) {
                context->advance(); // consume '@'

                std::string itemTypeStr = context->getCurrentToken().lexeme;
                context->advance();

                TemplateUsageType usageType;
                if (itemTypeStr == "Element") {
                    usageType = TemplateUsageType::ELEMENT;
                } else {
                    throw std::runtime_error("Only @Element templates can be used in this context. Found: @" + itemTypeStr);
                }

                std::string templateName = context->getCurrentToken().lexeme;
                context->advance();

                auto usageNode = std::make_shared<TemplateUsageNode>(templateName, usageType);

                if (context->getCurrentToken().type == TokenType::TOKEN_LBRACE) {
                    context->setStrategy(std::make_unique<SpecializationParsingStrategy>());
                    auto specializationRoot = context->runCurrentStrategy();
                    usageNode->setSpecialization(specializationRoot);
                } else if (context->getCurrentToken().type == TokenType::TOKEN_SEMICOLON) {
                    context->advance(); // consume ';'
                } else {
                    throw std::runtime_error("Expected ';' or '{' after template usage.");
                }

                element->addChild(usageNode);
            }
            else {
                context->advance();
            }
        }

        if (context->getCurrentToken().type == TokenType::TOKEN_RBRACE) {
            context->advance(); // Consume '}'
        } else {
            // Error: unclosed element
        }
    }

    context->setCurrentElement(previousElement);

    return element;
}

}