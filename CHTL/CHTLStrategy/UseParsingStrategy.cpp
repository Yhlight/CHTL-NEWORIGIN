#include "UseParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLNode/UseNode.h"
#include <stdexcept>

namespace CHTL {

std::shared_ptr<BaseNode> UseParsingStrategy::parse(CHTLParserContext* context) {
    // Consume 'use' token
    context->advance();

    if (context->getCurrentToken().type == TokenType::TOKEN_KEYWORD_HTML5) {
        std::string useType = context->getCurrentToken().lexeme;
        context->advance(); // Consume 'html5'

        if (context->getCurrentToken().type == TokenType::TOKEN_SEMICOLON) {
            context->advance(); // Consume ';'
            return std::make_shared<UseNode>(useType);
        } else {
            throw std::runtime_error("Expected ';' after use directive.");
        }
    } else {
        throw std::runtime_error("Expected 'html5' after 'use' keyword.");
    }

    return nullptr;
}

}