#include "InitialState.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLStrategy/ElementParsingStrategy.h"
#include "../CHTLStrategy/StyleParsingStrategy.h"
#include "../CHTLStrategy/TextParsingStrategy.h"
#include "../CHTLStrategy/TemplateParsingStrategy.h"
#include "../CHTLStrategy/CustomParsingStrategy.h"
#include "../CHTLStrategy/OriginParsingStrategy.h"
#include "../CHTLStrategy/ImportParsingStrategy.h"
#include "../CHTLStrategy/NamespaceParsingStrategy.h"
#include "../CHTLStrategy/ConfigurationParsingStrategy.h"


namespace CHTL {

    void InitialState::handleToken(CHTLParserContext* context, const Token& token) {
        switch (token.type) {
            case TokenType::TOKEN_IDENTIFIER:
                context->setStrategy(std::make_unique<ElementParsingStrategy>());
                break;
            case TokenType::TOKEN_STYLE:
                context->setStrategy(std::make_unique<StyleParsingStrategy>());
                break;
            case TokenType::TOKEN_TEXT:
                context->setStrategy(std::make_unique<TextParsingStrategy>());
                break;
            case TokenType::TOKEN_LBRACKET:
                if (context->peek(1).type == TokenType::TOKEN_IDENTIFIER && context->peek(1).lexeme == "Template") {
                    context->setStrategy(std::make_unique<TemplateParsingStrategy>());
                } else if (context->peek(1).type == TokenType::TOKEN_IDENTIFIER && context->peek(1).lexeme == "Custom") {
                    context->setStrategy(std::make_unique<CustomParsingStrategy>());
                } else if (context->peek(1).type == TokenType::TOKEN_IDENTIFIER && context->peek(1).lexeme == "Origin") {
                    context->setStrategy(std::make_unique<OriginParsingStrategy>());
                } else if (context->peek(1).type == TokenType::TOKEN_IDENTIFIER && context->peek(1).lexeme == "Import") {
                    context->setStrategy(std::make_unique<ImportParsingStrategy>());
                } else if (context->peek(1).type == TokenType::TOKEN_IDENTIFIER && context->peek(1).lexeme == "Namespace") {
                    context->setStrategy(std::make_unique<NamespaceParsingStrategy>());
                } else if (context->peek(1).type == TokenType::TOKEN_IDENTIFIER && context->peek(1).lexeme == "Configuration") {
                    context->setStrategy(std::make_unique<ConfigurationParsingStrategy>());
                }
                break;
            default:
                // Handle error or unknown token
                break;
        }
    }

}