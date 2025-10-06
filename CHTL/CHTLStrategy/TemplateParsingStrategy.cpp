#include "TemplateParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/TemplateUsageNode.h" // Add this include
#include "../CHTLParser/ParsingUtils.h"
#include "ElementParsingStrategy.h"
#include "TextParsingStrategy.h"
#include "StyleParsingStrategy.h"
#include "../CHTLNode/PropertyNode.h"
#include <stdexcept>

namespace CHTL {

std::shared_ptr<BaseNode> TemplateParsingStrategy::parse(CHTLParserContext* context) {
    context->advance(); // consume '['
    context->advance(); // consume 'Template'
    context->advance(); // consume ']'

    std::string itemTypeStr = "";
    if (context->getCurrentToken().type == TokenType::TOKEN_AT) {
        itemTypeStr += context->getCurrentToken().lexeme;
        context->advance(); // consume '@'
    }
    itemTypeStr += context->getCurrentToken().lexeme;
    context->advance(); // consume template type

    TemplateType templateType;
    if (itemTypeStr == "@Style") {
        templateType = TemplateType::STYLE;
    } else if (itemTypeStr == "@Element") {
        templateType = TemplateType::ELEMENT;
    } else if (itemTypeStr == "@Var") {
        templateType = TemplateType::VAR;
    } else {
        return nullptr;
    }

    std::string templateName = context->getCurrentToken().lexeme;
    context->advance(); // consume template name

    auto templateNode = std::make_shared<TemplateNode>(templateName, templateType);

    if (context->getCurrentToken().type == TokenType::TOKEN_LBRACE) {
        context->advance(); // consume '{'

        while (context->getCurrentToken().type != TokenType::TOKEN_RBRACE && !context->isAtEnd()) {
            if (templateType == TemplateType::STYLE || templateType == TemplateType::VAR) {

                bool is_inheritance = (context->getCurrentToken().lexeme == "inherit");
                if (is_inheritance) {
                    context->advance(); // consume 'inherit'
                }

                // Handle both implicit and explicit inheritance
                if (context->getCurrentToken().type == TokenType::TOKEN_AT) {
                    context->advance(); // consume '@'
                    std::string usageTypeStr = context->getCurrentToken().lexeme;
                    if (usageTypeStr != "Style") throw std::runtime_error("Only @Style templates can be inherited in a Style template.");
                    context->advance(); // consume type

                    std::string templateName = context->getCurrentToken().lexeme;
                    context->advance(); // consume name
                    templateNode->addChild(std::make_shared<TemplateUsageNode>(templateName, TemplateUsageType::STYLE));

                    if (context->getCurrentToken().type == TokenType::TOKEN_SEMICOLON) context->advance();
                    continue;
                }

                if (is_inheritance) {
                    // If we saw 'inherit', we must see '@Style'
                    throw std::runtime_error("Expected '@Style' after 'inherit'.");
                }

                // Handle Property Declaration
                if (context->getCurrentToken().type == TokenType::TOKEN_IDENTIFIER || context->getCurrentToken().type == TokenType::TOKEN_UNQUOTED_LITERAL) {
                    std::string key = context->getCurrentToken().lexeme;
                    context->advance(); // consume key

                    if (context->getCurrentToken().type != TokenType::TOKEN_COLON && context->getCurrentToken().type != TokenType::TOKEN_ASSIGN) {
                        throw std::runtime_error("Expected ':' or '=' after property key '" + key + "'.");
                    }
                    context->advance(); // consume ':' or '='

                    std::string value;
                    while (context->getCurrentToken().type != TokenType::TOKEN_SEMICOLON && context->getCurrentToken().type != TokenType::TOKEN_RBRACE && !context->isAtEnd()) {
                        value += context->getCurrentToken().lexeme;
                        const auto& next_token = context->peek(1);
                        if (next_token.type != TokenType::TOKEN_SEMICOLON && next_token.type != TokenType::TOKEN_RBRACE && next_token.type != TokenType::TOKEN_EOF) {
                            value += " ";
                        }
                        context->advance();
                    }

                    templateNode->addChild(std::make_shared<PropertyNode>(key, value));

                    if (context->getCurrentToken().type == TokenType::TOKEN_SEMICOLON) context->advance();
                    continue;
                }

                throw std::runtime_error("Unexpected token in template block: " + context->getCurrentToken().lexeme);

            } else if (templateType == TemplateType::ELEMENT) {
                TokenType currentType = context->getCurrentToken().type;
                if (currentType == TokenType::TOKEN_STYLE) {
                    context->setStrategy(std::make_unique<StyleParsingStrategy>());
                    templateNode->addChild(context->runCurrentStrategy());
                } else if (currentType == TokenType::TOKEN_TEXT) {
                    context->setStrategy(std::make_unique<TextParsingStrategy>());
                    templateNode->addChild(context->runCurrentStrategy());
                } else if (currentType == TokenType::TOKEN_IDENTIFIER) {
                    context->setStrategy(std::make_unique<ElementParsingStrategy>());
                    templateNode->addChild(context->runCurrentStrategy());
                } else {
                    context->advance();
                }
            } else {
                context->advance();
            }
        }

        if (context->getCurrentToken().type == TokenType::TOKEN_RBRACE) {
            context->advance(); // consume '}'
        } else {
            throw std::runtime_error("Unclosed template block: " + templateName);
        }
    }

    return templateNode;
}

}