#include "TextParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include <sstream>

namespace CHTL {

std::shared_ptr<BaseNode> TextParsingStrategy::parse(CHTLParserContext* context) {
    context->advance(); // consume 'text'

    auto textElement = std::make_shared<ElementNode>("text");
    std::stringstream content;

    if (context->getCurrentToken().type == TokenType::TOKEN_LBRACE) {
        context->advance(); // consume '{'

        while (context->getCurrentToken().type != TokenType::TOKEN_RBRACE && !context->isAtEnd()) {
            if (context->getCurrentToken().type == TokenType::TOKEN_STRING_LITERAL || context->getCurrentToken().type == TokenType::TOKEN_UNQUOTED_LITERAL) {
                content << context->getCurrentToken().lexeme;
            }
            context->advance();
        }

        if (context->getCurrentToken().type == TokenType::TOKEN_RBRACE) {
            context->advance(); // consume '}'
        } else {
            // Error: Unclosed text block
        }
    }

    textElement->addChild(std::make_shared<TextNode>(content.str()));
    return textElement;
}

}