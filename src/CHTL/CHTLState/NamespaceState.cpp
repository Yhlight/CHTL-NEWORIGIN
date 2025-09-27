#include "NamespaceState.h"
#include "StatementState.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLNode/NamespaceNode.h"
#include <stdexcept>

std::unique_ptr<BaseNode> NamespaceState::handle(Parser& parser) {
    // The initial KEYWORD_NAMESPACE token has already been consumed by StatementState
    // to transition into this state.

    // Expect the namespace name
    if (parser.currentToken.type != TokenType::Identifier) {
        throw std::runtime_error("Expected a name for the namespace.");
    }
    std::string namespaceName = parser.currentToken.value;
    parser.advanceTokens();

    parser.pushNamespace(namespaceName);

    // Check for an optional block
    if (parser.currentToken.type == TokenType::OpenBrace) {
        parser.advanceTokens(); // Consume '{'

        std::vector<std::unique_ptr<BaseNode>> children;

        // Temporarily set state to parse statements within the block
        auto previousState = std::make_unique<StatementState>();

        while (parser.currentToken.type != TokenType::CloseBrace && parser.currentToken.type != TokenType::EndOfFile) {
            auto node = previousState->handle(parser);
            if (node) {
                children.push_back(std::move(node));
            }
        }

        parser.expectToken(TokenType::CloseBrace); // Consume '}'

        parser.popNamespace();

        // After the block, transition back to the main statement state.
        parser.setState(std::make_unique<StatementState>());

        // Return a nullptr because the templates inside are registered,
        // and the namespace itself isn't a runtime element in the final output.
        // The namespace's effect is handled by the TemplateManager.
        return nullptr;

    } else {
        // No block, the namespace applies to the rest of the file (or until next namespace decl)
        // The new namespace is already on the stack.
        // We just need to go back to parsing statements.
        parser.setState(std::make_unique<StatementState>());
        return nullptr; // This is a directive, not an AST node to be returned
    }
}