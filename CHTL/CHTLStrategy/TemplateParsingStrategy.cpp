#include "TemplateParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLNode/TemplateNode.h"
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
                parseProperties(context, templateNode);
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