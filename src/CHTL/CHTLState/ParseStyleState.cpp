#include "ParseStyleState.h"
#include "ParseElementState.h"
#include "CHTLContext/Context.h"
#include "CHTLStrategy/ParseSimplePropertyStrategy.h"
#include <iostream>
#include <stdexcept>

namespace CHTL {

void ParseStyleState::handle(CHTLContext& context) {
    // This state is active inside a style { ... } block.
    while (context.peekToken().type != TokenType::RIGHT_BRACE &&
           context.peekToken().type != TokenType::END_OF_FILE) {

        // Here, the state's job is to determine which strategy to use.
        // For now, we only have one strategy for simple properties.
        // In the future, we could look at the token and decide to use
        // a different strategy for selectors, etc.
        context.setStrategy(std::make_unique<ParseSimplePropertyStrategy>());
        context.executeStrategy();
    }

    if (context.peekToken().type != TokenType::RIGHT_BRACE) {
        throw std::runtime_error("Expected '}' to close style block.");
    }
    context.consumeToken(); // Consume the '}'

    // After parsing a style block, we return to parsing the parent element.
    context.setState(std::make_unique<ParseElementState>());
}

} // namespace CHTL
