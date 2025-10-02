#include "TextParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLNode/TextNode.h"

namespace CHTL {

std::shared_ptr<BaseNode> TextParsingStrategy::parse(CHTLParserContext* context) {
    context->advance(); // consume 'text' keyword
    if (context->getCurrentToken().type == TokenType::TOKEN_LBRACE) {
        context->advance(); // consume '{'
        std::string content_text;
        if (context->getCurrentToken().type == TokenType::TOKEN_STRING_LITERAL || context->getCurrentToken().type == TokenType::TOKEN_UNQUOTED_LITERAL) {
            Token content = context->getCurrentToken();
            context->advance();
            content_text = content.lexeme;
        }
        if (context->getCurrentToken().type == TokenType::TOKEN_RBRACE) {
            context->advance(); // consume '}'
        } else {
            // Error: unclosed text block
        }
        return std::make_shared<TextNode>(content_text);
    }
    return nullptr;
}

}