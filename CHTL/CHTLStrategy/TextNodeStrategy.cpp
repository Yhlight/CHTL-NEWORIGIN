#include "TextNodeStrategy.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLLexer/Token.h"
#include <stdexcept>

namespace CHTL {

std::unique_ptr<BaseNode> TextNodeStrategy::parse(CHTLParser& parser) {
    parser.consume(TokenType::L_BRACE, "Expect '{' after 'text' keyword.");
    Token text = parser.consume(TokenType::STRING_LITERAL, "Expect string literal inside text block.");
    parser.consume(TokenType::R_BRACE, "Expect '}' after text block.");
    return std::make_unique<TextNode>(text.value);
}

} // namespace CHTL