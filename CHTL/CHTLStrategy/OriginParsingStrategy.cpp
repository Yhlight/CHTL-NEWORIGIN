#include "OriginParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLNode/OriginNode.h"

namespace CHTL {

std::shared_ptr<BaseNode> OriginParsingStrategy::parse(CHTLParserContext* context) {
    context->advance(); // consume '['
    context->advance(); // consume 'Origin'
    context->advance(); // consume ']'

    std::string originType = context->getCurrentToken().lexeme;
    context->advance(); // consume origin type

    std::string name = "";
    if (context->getCurrentToken().type == TokenType::TOKEN_IDENTIFIER) {
        name = context->getCurrentToken().lexeme;
        context->advance(); // consume name
    }

    std::string content = "";
    if (context->getCurrentToken().type == TokenType::TOKEN_LBRACE) {
        context->advance(); // consume '{'

        while (context->getCurrentToken().type != TokenType::TOKEN_RBRACE && !context->isAtEnd()) {
            content += context->getCurrentToken().lexeme;
            context->advance();
        }

        if (context->getCurrentToken().type == TokenType::TOKEN_RBRACE) {
            context->advance(); // consume '}'
        } else {
            // Error: unclosed origin block
        }
    }

    return std::make_shared<OriginNode>(originType, name, content);
}

}