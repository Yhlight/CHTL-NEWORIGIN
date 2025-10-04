#include "TemplateParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLNode/TemplateNode.h"
#include "StyleParsingStrategy.h"
#include "TextParsingStrategy.h"
#include "ElementParsingStrategy.h"
#include "../CHTLNode/PropertyNode.h"

namespace CHTL {

std::shared_ptr<BaseNode> TemplateParsingStrategy::parse(CHTLParserContext* context) {
    context->advance(); // consume '['
    context->advance(); // consume 'Template'
    context->advance(); // consume ']'

    TemplateType templateType;
    if (context->getCurrentToken().lexeme == "@Style") {
        templateType = TemplateType::STYLE;
    } else if (context->getCurrentToken().lexeme == "@Element") {
        templateType = TemplateType::ELEMENT;
    } else if (context->getCurrentToken().lexeme == "@Var") {
        templateType = TemplateType::VAR;
    } else {
        // Error: invalid template type
        return nullptr;
    }
    context->advance(); // consume template type

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
            // Error: unclosed template block
        }
    }

    return templateNode;
}

}