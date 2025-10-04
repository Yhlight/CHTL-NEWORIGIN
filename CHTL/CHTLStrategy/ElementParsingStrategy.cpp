#include "ElementParsingStrategy.h"
#include "TextParsingStrategy.h"
#include "StyleParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"

namespace CHTL {

void parseAttributes(CHTLParserContext* context, std::shared_ptr<ElementNode> element) {
    while (context->getCurrentToken().type == TokenType::TOKEN_IDENTIFIER) {
        if (context->peek(1).type == TokenType::TOKEN_COLON || context->peek(1).type == TokenType::TOKEN_ASSIGN) {
            std::string key = context->getCurrentToken().lexeme;
            context->advance(); // consume identifier
            context->advance(); // consume ':' or '='
            if (context->getCurrentToken().type == TokenType::TOKEN_STRING_LITERAL || context->getCurrentToken().type == TokenType::TOKEN_UNQUOTED_LITERAL || context->getCurrentToken().type == TokenType::TOKEN_NUMERIC_LITERAL) {
                std::string value = context->getCurrentToken().lexeme;
                context->advance(); // consume value
                element->setAttribute(key, value);
                if (context->getCurrentToken().type == TokenType::TOKEN_SEMICOLON) {
                    context->advance(); // consume ';'
                }
            } else {
                // Error: unexpected token in attribute value
                break;
            }
        } else {
            // Not an attribute, break the loop
            break;
        }
    }
}


std::shared_ptr<BaseNode> ElementParsingStrategy::parse(CHTLParserContext* context) {
    Token identifier = context->getCurrentToken();
    context->advance();
    auto element = std::make_shared<ElementNode>(identifier.lexeme);

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
            } else if (currentType == TokenType::TOKEN_IDENTIFIER) {
                if (context->peek(1).type == TokenType::TOKEN_COLON || context->peek(1).type == TokenType::TOKEN_ASSIGN) {
                    parseAttributes(context, element);
                } else {
                    context->setStrategy(std::make_unique<ElementParsingStrategy>());
                    element->addChild(context->runCurrentStrategy());
                }
            } else {
                context->advance();
            }
        }

        if (context->getCurrentToken().type == TokenType::TOKEN_RBRACE) {
            context->advance(); // Consume '}'
        } else {
            // Error: unclosed element
        }
    }

    return element;
}

}