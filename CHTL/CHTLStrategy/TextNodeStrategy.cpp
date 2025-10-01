#include "TextNodeStrategy.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLLexer/Token.h"

namespace CHTL {

void TextNodeStrategy::execute(CHTLParser& parser) {
    // The state has already consumed the 'text' keyword.

    // 1. Consume the opening '{' brace.
    parser.consume(TokenType::L_BRACE, "Expected '{' after 'text' keyword.");

    // 2. Consume the string literal.
    Token textToken = parser.consume(TokenType::STRING_LITERAL, "Expected a string literal for text content.");

    // 3. Create a new TextNode.
    auto textNode = std::make_unique<TextNode>(textToken.value);

    // 4. Add the new node to the current parent.
    parser.addNode(std::move(textNode));

    // 5. Consume the closing '}' brace.
    parser.consume(TokenType::R_BRACE, "Expected '}' after text content.");
}

} // namespace CHTL