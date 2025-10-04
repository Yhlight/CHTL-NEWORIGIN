#include "CustomParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLNode/CustomNode.h"
#include "../CHTLParser/ParsingUtils.h"
#include "ElementParsingStrategy.h"
#include "TextParsingStrategy.h"
#include "StyleParsingStrategy.h"
#include "../CHTLNode/PropertyNode.h"

namespace CHTL {

std::shared_ptr<BaseNode> CustomParsingStrategy::parse(CHTLParserContext* context) {
    context->advance(); // consume '['
    context->advance(); // consume 'Custom'
    context->advance(); // consume ']'

    CustomType customType;
    if (context->getCurrentToken().lexeme == "@Style") {
        customType = CustomType::STYLE;
    } else if (context->getCurrentToken().lexeme == "@Element") {
        customType = CustomType::ELEMENT;
    } else if (context->getCurrentToken().lexeme == "@Var") {
        customType = CustomType::VAR;
    } else {
        // Error: invalid custom type
        return nullptr;
    }
    context->advance(); // consume custom type

    std::string customName = context->getCurrentToken().lexeme;
    context->advance(); // consume custom name

    auto customNode = std::make_shared<CustomNode>(customName, customType);

    if (context->getCurrentToken().type == TokenType::TOKEN_LBRACE) {
        context->advance(); // consume '{'

        while (context->getCurrentToken().type != TokenType::TOKEN_RBRACE && !context->isAtEnd()) {
            if (customType == CustomType::STYLE || customType == CustomType::VAR) {
                parseProperties(context, customNode);
            } else if (customType == CustomType::ELEMENT) {
                TokenType currentType = context->getCurrentToken().type;
                if (currentType == TokenType::TOKEN_STYLE) {
                    context->setStrategy(std::make_unique<StyleParsingStrategy>());
                    customNode->addChild(context->runCurrentStrategy());
                } else if (currentType == TokenType::TOKEN_TEXT) {
                    context->setStrategy(std::make_unique<TextParsingStrategy>());
                    customNode->addChild(context->runCurrentStrategy());
                } else if (currentType == TokenType::TOKEN_IDENTIFIER) {
                    context->setStrategy(std::make_unique<ElementParsingStrategy>());
                    customNode->addChild(context->runCurrentStrategy());
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
            // Error: unclosed custom block
        }
    }

    return customNode;
}

}