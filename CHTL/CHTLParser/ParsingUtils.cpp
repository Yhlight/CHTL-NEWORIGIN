#include "ParsingUtils.h"
#include "CHTLParserContext.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/PropertyNode.h"

namespace CHTL {

void parseProperties(CHTLParserContext* context, std::shared_ptr<BaseNode> parentNode) {
    while (context->getCurrentToken().type == TokenType::TOKEN_IDENTIFIER || context->getCurrentToken().type == TokenType::TOKEN_UNQUOTED_LITERAL) {
        std::string key = context->getCurrentToken().lexeme;
        context->advance();

        if (context->getCurrentToken().type == TokenType::TOKEN_COLON || context->getCurrentToken().type == TokenType::TOKEN_ASSIGN) {
            context->advance(); // consume ':' or '='

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

}