#include "AttributeStrategy.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLLexer/Token.h"
#include <stdexcept>

namespace CHTL {

std::unique_ptr<BaseNode> AttributeStrategy::parse(CHTLParser& parser) {
    BaseNode* current_node = parser.currentNode();
    if (!current_node) {
        throw std::runtime_error("Attempting to parse attribute outside of an element context.");
    }

    auto* element = dynamic_cast<ElementNode*>(current_node);
    if (!element) {
        throw std::runtime_error("Attempting to parse attribute on a non-element node.");
    }

    Token key = parser.consume(TokenType::IDENTIFIER, "Expect attribute key.");
    if (parser.match(TokenType::COLON) || parser.match(TokenType::EQUAL)) {
        std::string value_str;
        while (!parser.isAtEnd() && parser.peek().type != TokenType::SEMICOLON) {
            value_str += parser.advance().value;
        }
        if (value_str.empty()) throw std::runtime_error("Expect attribute value.");
        element->addAttribute(key.value, value_str);
        parser.consume(TokenType::SEMICOLON, "Expect ';' after attribute.");
    } else {
        throw std::runtime_error("Expect ':' or '=' after attribute key.");
    }

    // This strategy modifies the current node, it does not return a new one.
    return nullptr;
}

} // namespace CHTL