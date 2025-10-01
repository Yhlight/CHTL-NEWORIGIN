#include "AttributeStrategy.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLLexer/Token.h"
#include <stdexcept>

namespace CHTL {

void AttributeStrategy::execute(CHTLParser& parser) {
    // The BodyParsingState has already seen an IDENTIFIER, so we consume it as the key.
    Token key = parser.consume(TokenType::IDENTIFIER, "Expected attribute key.");

    // Consume the separator (':' or '=').
    if (!parser.match(TokenType::COLON) && !parser.match(TokenType::EQUAL)) {
        throw std::runtime_error("Expected ':' or '=' after attribute key.");
    }

    // Consume the value. This can be a single string literal or a sequence of other tokens
    // that are treated as an unquoted literal.
    std::string value_str;
    if (parser.peek().type == TokenType::STRING_LITERAL) {
        value_str = parser.consume(TokenType::STRING_LITERAL, "Expected string literal.").value;
    } else {
        // It's an unquoted literal. Consume tokens until we see the semicolon.
        while (!parser.isAtEnd() && parser.peek().type != TokenType::SEMICOLON) {
            value_str += parser.advance().value;
        }
    }

    if (value_str.empty()) {
        throw std::runtime_error("Attribute value cannot be empty.");
    }

    // Consume the trailing semicolon.
    parser.consume(TokenType::SEMICOLON, "Expected ';' after attribute value.");

    // Get the current element node and add the attribute.
    ElementNode* currentNode = parser.getCurrentNode();
    if (currentNode) {
        currentNode->addAttribute(key.value, value_str);
    } else {
        throw std::runtime_error("Cannot add attribute, no current element node.");
    }
}

} // namespace CHTL