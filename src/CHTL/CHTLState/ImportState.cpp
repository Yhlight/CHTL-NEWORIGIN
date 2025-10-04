#include "ImportState.h"
#include "DefaultState.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/ImportNode.h"
#include <stdexcept>

void ImportState::handle(CHTLParser& parser, Token token) {
    // The [Import] token has been consumed by the DefaultState.
    // The token passed to this handle method should be the AT_SIGN.
    if (token.type != TokenType::AT_SIGN) {
        // If it's not an AT_SIGN, it's an error. We'll put the token back and switch to DefaultState
        // to avoid consuming a token that another state might need.
        parser.putback(token);
        parser.setState(std::make_unique<DefaultState>());
        // Optionally, throw an exception or log an error
        throw std::runtime_error("Expected @ after [Import]");
        return;
    }

    Token type = parser.consume();
    if (type.type != TokenType::IDENTIFIER || type.value != "Chtl") {
        throw std::runtime_error("Expected 'Chtl' after @");
    }

    Token from = parser.consume();
    if (from.type != TokenType::KEYWORD_FROM) {
        throw std::runtime_error("Expected 'from' keyword in import statement");
    }

    Token path = parser.consume();
    if (path.type != TokenType::STRING_LITERAL) {
        throw std::runtime_error("Expected file path string literal in import statement");
    }

    // Optional semicolon
    if (parser.peek().type == TokenType::SEMICOLON) {
        parser.consume();
    }

    auto importNode = std::make_unique<ImportNode>(path.value);
    parser.addNode(std::move(importNode));

    parser.setState(std::make_unique<DefaultState>());
}