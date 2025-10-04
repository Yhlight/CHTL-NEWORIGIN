#include "NamespaceState.h"
#include "DefaultState.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/NamespaceNode.h"
#include <stdexcept>

void NamespaceState::handle(CHTLParser& parser, Token token) {
    // We've already consumed [Namespace].
    // The token passed should be the namespace name.
    if (token.type != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected namespace name after [Namespace]");
    }
    std::string name = token.value;

    auto namespaceNode = std::make_unique<NamespaceNode>(name);

    // Check for an opening brace to see if there's a body
    if (parser.peek().type == TokenType::LEFT_BRACE) {
        parser.consume(); // Consume '{'
        auto* namespaceNodePtr = namespaceNode.get();
        parser.addNode(std::move(namespaceNode));
        parser.openScope(namespaceNodePtr);
        // The DefaultState will handle the content inside the namespace
    } else {
        // No body, just a declaration. Add the node and continue.
        parser.addNode(std::move(namespaceNode));
    }

    parser.setState(std::make_unique<DefaultState>());
}