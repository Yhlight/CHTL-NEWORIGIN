#include "ImportState.h"
#include "DefaultState.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/ImportNode.h"
#include <stdexcept>

void ImportState::handle(CHTLParser& parser, Token token) {
    // The [Import] token has been consumed by the DefaultState.
    // The token passed to this handle method should be the AT_SIGN.
    if (token.type != TokenType::AT_SIGN) {
        parser.putback(token);
        parser.setState(std::make_unique<DefaultState>());
        throw std::runtime_error("Expected @ after [Import]");
        return;
    }

    Token typeToken = parser.consume();
    if (typeToken.type != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected import type (e.g., Chtl, Html) after @");
    }
    std::string importType = typeToken.value;

    Token fromToken = parser.consume();
    if (fromToken.type != TokenType::KEYWORD_FROM) {
        throw std::runtime_error("Expected 'from' keyword in import statement");
    }

    Token pathToken = parser.consume();
    if (pathToken.type != TokenType::STRING_LITERAL) {
        throw std::runtime_error("Expected file path string literal in import statement");
    }

    std::string alias;
    if (parser.peek().type == TokenType::KEYWORD_AS) {
        parser.consume(); // Consume 'as'
        Token aliasToken = parser.consume();
        if (aliasToken.type != TokenType::IDENTIFIER) {
            throw std::runtime_error("Expected alias identifier after 'as'");
        }
        alias = aliasToken.value;
    } else if (importType != "Chtl") {
        // 'as' is mandatory for non-Chtl imports
        throw std::runtime_error("The 'as' keyword is mandatory for importing " + importType + " files.");
    }

    // Optional semicolon
    if (parser.peek().type == TokenType::SEMICOLON) {
        parser.consume();
    }

    auto importNode = std::make_unique<ImportNode>(importType, pathToken.value, alias);
    parser.addNode(std::move(importNode));

    parser.setState(std::make_unique<DefaultState>());
}