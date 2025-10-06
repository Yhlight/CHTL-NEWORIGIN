#include "CustomParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLNode/CustomNode.h"
#include "../CHTLParser/ParsingUtils.h"
#include "ElementParsingStrategy.h"
#include "TextParsingStrategy.h"
#include "StyleParsingStrategy.h"
#include "../CHTLNode/PropertyNode.h"
#include <stdexcept>

namespace CHTL {

std::shared_ptr<BaseNode> CustomParsingStrategy::parse(CHTLParserContext* context) {
    context->advance(); // consume '['
    context->advance(); // consume 'Custom'
    context->advance(); // consume ']'

    std::string itemTypeStr = "";
    if (context->getCurrentToken().type == TokenType::TOKEN_AT) {
        itemTypeStr += context->getCurrentToken().lexeme;
        context->advance(); // consume '@'
    }
    itemTypeStr += context->getCurrentToken().lexeme;
    context->advance(); // consume custom type

    CustomType customType;
    if (itemTypeStr == "@Style") {
        customType = CustomType::STYLE;
    } else if (itemTypeStr == "@Element") {
        customType = CustomType::ELEMENT;
    } else if (itemTypeStr == "@Var") {
        customType = CustomType::VAR;
    } else {
        return nullptr;
    }

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
            throw std::runtime_error("Unclosed custom block: " + customName);
        }
    }

    return customNode;
}

}