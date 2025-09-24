#include "ParseSimplePropertyStrategy.h"
#include "CHTLContext/Context.h"
#include "CHTLNode/StyleBlockNode.h"
#include "CHTLNode/StylePropertyNode.h"
#include <stdexcept>

namespace CHTL {

void ParseSimplePropertyStrategy::execute(CHTLContext& context) {
    // 1. Consume the property name (identifier)
    Token keyToken = context.consumeToken();
    if (keyToken.type != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected a style property name.");
    }

    // 2. Consume the colon
    if (context.consumeToken().type != TokenType::COLON) {
        throw std::runtime_error("Expected ':' after style property name.");
    }

    // 3. Consume the value.
    Token valueToken = context.consumeToken();
    if (valueToken.type != TokenType::IDENTIFIER && valueToken.type != TokenType::UNQUOTED_LITERAL) {
         throw std::runtime_error("Expected a style property value.");
    }

    // 4. Consume the semicolon
    if (context.consumeToken().type != TokenType::SEMICOLON) {
        throw std::runtime_error("Expected ';' after style property value.");
    }

    // 5. Get the current style block and add the new property node to it.
    StyleBlockNode* styleBlock = context.getCurrentStyleBlock();
    if (styleBlock) {
        auto propNode = std::make_unique<StylePropertyNode>(keyToken.lexeme, valueToken.lexeme);
        styleBlock->properties.push_back(std::move(propNode));
    } else {
        // This would indicate a logic error in the parser, where a style parsing
        // strategy is being used outside of a style block context.
        throw std::logic_error("Attempted to parse style property without an active style block.");
    }
}

} // namespace CHTL
