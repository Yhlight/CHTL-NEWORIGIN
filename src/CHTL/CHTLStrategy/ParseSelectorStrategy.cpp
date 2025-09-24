// FIX: Reorder includes to ensure full type definitions are available before use.
#include "CHTLNode/SelectorBlockNode.h"
#include "CHTLNode/StyleBlockNode.h"

#include "ParseSelectorStrategy.h"
#include "ParseSimplePropertyStrategy.h"
#include "CHTLContext/Context.h"
#include <stdexcept>

namespace CHTL {

void ParseSelectorStrategy::execute(CHTLContext& context) {
    std::string selector_name;
    Token current = context.consumeToken();
    selector_name += current.lexeme;

    if (context.peekToken().type == TokenType::IDENTIFIER) {
        selector_name += context.consumeToken().lexeme;
    }

    auto selectorNode = std::make_unique<SelectorBlockNode>(selector_name);

    if (context.consumeToken().type != TokenType::LEFT_BRACE) {
        throw std::runtime_error("Expected '{' after selector name.");
    }

    context.pushStyleContainer(selectorNode.get());

    auto propertyStrategy = std::make_unique<ParseSimplePropertyStrategy>();
    while(context.peekToken().type != TokenType::RIGHT_BRACE && context.peekToken().type != TokenType::END_OF_FILE) {
        propertyStrategy->execute(context);
    }

    if (context.consumeToken().type != TokenType::RIGHT_BRACE) {
        throw std::runtime_error("Expected '}' to close selector block.");
    }

    context.popStyleContainer();

    BaseNode* parentContainer = context.getCurrentStyleContainer();
    if (auto* parentStyleBlock = dynamic_cast<StyleBlockNode*>(parentContainer)) {
        parentStyleBlock->rules.push_back(std::move(selectorNode));
    } else {
        throw std::logic_error("Attempted to parse selector block without an active parent style block.");
    }
}

} // namespace CHTL
