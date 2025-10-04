#include "OriginParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLNode/OriginNode.h"
#include <sstream>

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

    std::stringstream contentStream;
    if (context->getCurrentToken().type == TokenType::TOKEN_LBRACE) {
        context->advance(); // consume '{'

        bool firstToken = true;
        while (context->getCurrentToken().type != TokenType::TOKEN_RBRACE && !context->isAtEnd()) {
            if (!firstToken) {
                contentStream << " ";
            }
            contentStream << context->getCurrentToken().lexeme;
            firstToken = false;
            context->advance();
        }

        if (context->getCurrentToken().type == TokenType::TOKEN_RBRACE) {
            context->advance(); // consume '}'
        } else {
            // Error: unclosed origin block
        }
    }

    return std::make_shared<OriginNode>(originType, name, contentStream.str());
}
}