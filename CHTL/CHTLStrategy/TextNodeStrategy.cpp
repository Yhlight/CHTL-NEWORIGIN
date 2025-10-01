#include "TextNodeStrategy.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLLexer/Token.h"
#include <stdexcept>

namespace CHTL {

std::unique_ptr<BaseNode> TextNodeStrategy::parse(CHTLParser& parser) {
    parser.consume(TokenType::L_BRACE, "Expect '{' after 'text' keyword.");

    std::string text_content;

    if (parser.check(TokenType::R_BRACE)) {
        // Empty text block. Content remains empty.
    } else if (parser.match(TokenType::STRING_LITERAL)) {
        // Quoted string literal.
        text_content = parser.previous().value;
    } else {
        // Unquoted text (sequence of tokens).
        std::string full_text;
        while (!parser.check(TokenType::R_BRACE) && !parser.isAtEnd()) {
            if (!full_text.empty()) {
                // Avoid adding a space before punctuation like a dot.
                if (parser.peek().type != TokenType::DOT) {
                    full_text += " ";
                }
            }
            full_text += parser.advance().value;
        }
        text_content = full_text;
    }

    parser.consume(TokenType::R_BRACE, "Expect '}' after text block.");
    return std::make_unique<TextNode>(text_content);
}

} // namespace CHTL