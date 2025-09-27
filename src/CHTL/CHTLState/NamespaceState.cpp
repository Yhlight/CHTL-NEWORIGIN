#include "NamespaceState.h"
#include "StatementState.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLNode/NamespaceNode.h"

std::unique_ptr<BaseNode> NamespaceState::handle(Parser& parser) {
    // The initial '[', 'Namespace', ']' tokens are consumed by StatementState
    // before transitioning to this state.

    std::string ns_name = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    auto namespaceNode = std::make_unique<NamespaceNode>(ns_name);

    // Push the new namespace onto the stack, making it the active namespace
    parser.namespaceStack.push_back(ns_name);

    // Check for an optional body
    if (parser.currentToken.type == TokenType::OpenBrace) {
        parser.advanceTokens(); // Consume '{'

        // Recursively parse the statements within the namespace body
        while (parser.currentToken.type != TokenType::CloseBrace && parser.currentToken.type != TokenType::EndOfFile) {
            StatementState statementParser;
            auto childNode = statementParser.handle(parser);
            if (childNode) {
                namespaceNode->addChild(std::move(childNode));
            }
        }
        parser.expectToken(TokenType::CloseBrace);
    } else if (parser.currentToken.type == TokenType::Semicolon) {
        // Handle bodiless namespace declaration: '[Namespace] my_ns;'
        parser.advanceTokens();
    } else {
        // This handles the case of a namespace declaration without a body or a semicolon.
        // It's not an error; it just means the namespace is active until the end of the file or the next pop.
    }


    // After parsing the body (or if there is no body), pop the namespace from the stack.
    if (!parser.namespaceStack.empty()) {
        parser.namespaceStack.pop_back();
    }

    return namespaceNode;
}