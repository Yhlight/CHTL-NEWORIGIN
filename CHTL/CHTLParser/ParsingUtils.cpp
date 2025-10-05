#include "ParsingUtils.h"
#include "CHTLParserContext.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/PropertyNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include <stdexcept>

namespace CHTL {

void parseProperties(CHTLParserContext* context, std::shared_ptr<BaseNode> parentNode) {
    while (context->getCurrentToken().type == TokenType::TOKEN_IDENTIFIER || context->getCurrentToken().type == TokenType::TOKEN_UNQUOTED_LITERAL) {
        std::string key = context->getCurrentToken().lexeme;
        context->advance();

        // Handle multi-part property names like "background-color"
        while (context->getCurrentToken().type == TokenType::TOKEN_MINUS &&
               (context->peek(1).type == TokenType::TOKEN_IDENTIFIER || context->peek(1).type == TokenType::TOKEN_UNQUOTED_LITERAL)) {
            key += "-";
            context->advance(); // consume '-'
            key += context->getCurrentToken().lexeme;
            context->advance(); // consume identifier
        }

        if (context->getCurrentToken().type == TokenType::TOKEN_COLON || context->getCurrentToken().type == TokenType::TOKEN_ASSIGN) {
            context->advance(); // consume ':' or '='

            // Check for variable template usage, e.g., ThemeColor(tableColor)
            if (context->getCurrentToken().type == TokenType::TOKEN_IDENTIFIER && context->peek(1).type == TokenType::TOKEN_LPAREN) {
                std::string templateName = context->getCurrentToken().lexeme;
                context->advance(); // consume template name
                context->advance(); // consume '('

                std::string variableName = context->getCurrentToken().lexeme;
                context->advance(); // consume variable name

                if (context->getCurrentToken().type != TokenType::TOKEN_RPAREN) {
                    throw std::runtime_error("Expected ')' to close variable template usage.");
                }
                context->advance(); // consume ')'

                auto propNode = std::make_shared<PropertyNode>(key, ""); // Value will be resolved by generator
                propNode->addChild(std::make_shared<TemplateUsageNode>(templateName, TemplateUsageType::VAR, variableName));
                parentNode->addChild(propNode);

                if (context->getCurrentToken().type == TokenType::TOKEN_SEMICOLON) {
                    context->advance(); // consume ';'
                }
            }
            // Regular property value
            else if (context->getCurrentToken().type == TokenType::TOKEN_STRING_LITERAL ||
                     context->getCurrentToken().type == TokenType::TOKEN_UNQUOTED_LITERAL ||
                     context->getCurrentToken().type == TokenType::TOKEN_NUMERIC_LITERAL ||
                     context->getCurrentToken().type == TokenType::TOKEN_IDENTIFIER) {
                std::string value = context->getCurrentToken().lexeme;
                context->advance();
                auto propNode = std::make_shared<PropertyNode>(key, value);
                parentNode->addChild(propNode);

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

}