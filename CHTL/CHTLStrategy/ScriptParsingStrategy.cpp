#include "ScriptParsingStrategy.h"
#include "AnimateParsingStrategy.h"
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

        auto scriptNode = std::make_shared<ScriptNode>();
        std::string current_content;
        int brace_level = 1;

        while (!context->isAtEnd()) {
            Token& current_token = context->getCurrentToken();

            // Check for Animate at the top level of the script block
            if (brace_level == 1 && current_token.type == TokenType::TOKEN_IDENTIFIER && current_token.lexeme == "Animate") {
                if (!current_content.empty()) {
                    scriptNode->addChild(std::make_shared<ScriptNode>(current_content));
                    current_content.clear();
                }

                AnimateParsingStrategy animate_parser;
                auto animate_node = animate_parser.parse(context);
                scriptNode->addChild(animate_node);
                continue; // Animate parser consumes its own tokens, so we restart the loop
            }

            if (current_token.type == TokenType::TOKEN_LBRACE) {
                brace_level++;
            } else if (current_token.type == TokenType::TOKEN_RBRACE) {
                brace_level--;
                if (brace_level == 0) {
                    break; // End of script block
                }
            }

            // Accumulate raw script content, without adding extra spaces
            if (current_token.type == TokenType::TOKEN_STRING_LITERAL) {
                current_content += "\"" + current_token.lexeme + "\"";
            } else {
                current_content += current_token.lexeme;
            }
            context->advance();
        }

        if (!current_content.empty()) {
            scriptNode->addChild(std::make_shared<ScriptNode>(current_content));
        }

        if (brace_level != 0) {
            throw std::runtime_error("Unclosed script block.");
        }

        context->advance(); // consume final '}'

        return scriptNode;
    }
}