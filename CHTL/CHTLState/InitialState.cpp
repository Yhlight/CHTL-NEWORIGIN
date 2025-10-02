#include "InitialState.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLStrategy/ElementParsingStrategy.h"
#include "../CHTLStrategy/StyleParsingStrategy.h"
#include "../CHTLStrategy/TextParsingStrategy.h"


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
            default:
                // Handle error or unknown token
                break;
        }
    }

}