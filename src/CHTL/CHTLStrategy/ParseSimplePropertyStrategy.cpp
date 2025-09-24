// FIX: Reorder includes to ensure full type definitions are available before use.
#include "CHTLNode/StylePropertyNode.h"
#include "CHTLNode/SelectorBlockNode.h"
#include "CHTLNode/StyleBlockNode.h"

#include "ParseSimplePropertyStrategy.h"
#include "CHTLContext/Context.h"
#include <stdexcept>

namespace CHTL {

void ParseSimplePropertyStrategy::execute(CHTLContext& context) {
    Token keyToken = context.consumeToken();
    if (keyToken.type != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected a style property name.");
    }

    if (context.consumeToken().type != TokenType::COLON) {
        throw std::runtime_error("Expected ':' after style property name.");
    }

    Token valueToken = context.consumeToken();
    if (valueToken.type != TokenType::IDENTIFIER && valueToken.type != TokenType::UNQUOTED_LITERAL) {
         throw std::runtime_error("Expected a style property value.");
    }

    if (context.consumeToken().type != TokenType::SEMICOLON) {
        throw std::runtime_error("Expected ';' after style property value.");
    }

    BaseNode* container = context.getCurrentStyleContainer();
    if (!container) {
        throw std::logic_error("Attempted to parse style property without an active style container.");
    }

    auto propNode = std::make_unique<StylePropertyNode>(keyToken.lexeme, valueToken.lexeme);

    if (auto* styleBlock = dynamic_cast<StyleBlockNode*>(container)) {
        styleBlock->rules.push_back(std::move(propNode));
    } else if (auto* selectorBlock = dynamic_cast<SelectorBlockNode*>(container)) {
        // SelectorBlockNode holds StylePropertyNodes directly in its 'properties' vector
        selectorBlock->properties.push_back(std::move(propNode));
    }
}

} // namespace CHTL
