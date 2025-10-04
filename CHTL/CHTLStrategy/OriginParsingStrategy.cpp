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

        int braceCount = 1;
        bool firstToken = true;

        while (!context->isAtEnd() && braceCount > 0) {
            Token& currentToken = context->getCurrentToken();

            if (currentToken.type == TokenType::TOKEN_LBRACE) {
                braceCount++;
            } else if (currentToken.type == TokenType::TOKEN_RBRACE) {
                braceCount--;
            }

            if (braceCount > 0) {
                if (!firstToken) {
                    contentStream << " ";
                }
                contentStream << currentToken.lexeme;
                firstToken = false;
            }
            context->advance();
        }
    }

    return std::make_shared<OriginNode>(originType, name, contentStream.str());
}
}