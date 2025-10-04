#include "StyleParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/PropertyNode.h"

namespace CHTL {

void parseProperties(CHTLParserContext* context, std::shared_ptr<BaseNode> parentNode) {
    while (context->getCurrentToken().type == TokenType::TOKEN_IDENTIFIER || context->getCurrentToken().type == TokenType::TOKEN_UNQUOTED_LITERAL) {
        std::string key = context->getCurrentToken().lexeme;
        context->advance();

        if (context->getCurrentToken().type == TokenType::TOKEN_COLON) {
            context->advance(); // consume ':'

            if (context->getCurrentToken().type == TokenType::TOKEN_STRING_LITERAL ||
                context->getCurrentToken().type == TokenType::TOKEN_UNQUOTED_LITERAL ||
                context->getCurrentToken().type == TokenType::TOKEN_NUMERIC_LITERAL ||
                context->getCurrentToken().type == TokenType::TOKEN_IDENTIFIER) {
                std::string value = context->getCurrentToken().lexeme;
                context->advance();
                parentNode->addChild(std::make_shared<PropertyNode>(key, value));

                if (context->getCurrentToken().type == TokenType::TOKEN_SEMICOLON) {
                    context->advance(); // consume ';'
                }
            } else {
                // Error handling
                break;
            }
        } else {
            // Error handling
            break;
        }
    }
}


std::shared_ptr<BaseNode> StyleParsingStrategy::parse(CHTLParserContext* context) {
    context->advance(); // consume 'style' keyword
    auto styleNode = std::make_shared<StyleNode>();

    if (context->getCurrentToken().type == TokenType::TOKEN_LBRACE) {
        context->advance(); // consume '{'

        parseProperties(context, styleNode);

        if (context->getCurrentToken().type == TokenType::TOKEN_RBRACE) {
            context->advance(); // consume '}'
        } else {
            // Error: unclosed style block
        }
    }

    return styleNode;
}

}