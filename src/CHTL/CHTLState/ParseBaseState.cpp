#include "ParseBaseState.h"
#include "ParseElementState.h"
#include "CHTLContext/Context.h"
#include "CHTLNode/ElementNode.h"
#include <stdexcept>

namespace CHTL {

void ParseBaseState::handle(CHTLContext& context) {
    Token currentToken = context.peekToken();

    if (currentToken.type == TokenType::IDENTIFIER) {
        context.consumeToken(); // Consume the identifier
        context.createRootNode(currentToken.lexeme);

        // Expect a left brace.
        Token nextToken = context.consumeToken();
        if (nextToken.type != TokenType::LEFT_BRACE) {
            throw std::runtime_error("Expected '{' after top-level element name.");
        }

        context.setState(std::make_unique<ParseElementState>());

    } else if (currentToken.type == TokenType::END_OF_FILE) {
        // This is fine, an empty file.
        context.consumeToken();
        return;
    } else {
        throw std::runtime_error("Unexpected token at top level: " + currentToken.lexeme);
    }
}

} // namespace CHTL
