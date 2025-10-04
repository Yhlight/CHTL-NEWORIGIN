#include "ScriptParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLNode/ScriptNode.h"
#include <stdexcept>

namespace CHTL {

std::shared_ptr<BaseNode> ScriptParsingStrategy::parse(CHTLParserContext* context) {
    context->advance(); // consume 'script'

    if (context->getCurrentToken().type != TokenType::TOKEN_LBRACE) {
        throw std::runtime_error("Expected '{' to start script block.");
    }
    context->advance(); // consume '{'

    std::string script_content;
    int brace_level = 1;

    while (!context->isAtEnd()) {
        Token& current_token = context->getCurrentToken();
        if (current_token.type == TokenType::TOKEN_LBRACE) {
            brace_level++;
        } else if (current_token.type == TokenType::TOKEN_RBRACE) {
            brace_level--;
            if (brace_level == 0) {
                break; // End of script block
            }
        }

        if (current_token.type == TokenType::TOKEN_STRING_LITERAL) {
            script_content += "\"" + current_token.lexeme + "\"";
        } else {
            script_content += current_token.lexeme;
        }

        context->advance();
    }

    if (brace_level != 0) {
        throw std::runtime_error("Unclosed script block.");
    }

    context->advance(); // consume final '}'

    return std::make_shared<ScriptNode>(script_content);
}

}