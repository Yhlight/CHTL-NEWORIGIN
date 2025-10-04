#include "ImportState.h"
#include "DefaultState.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/ImportNode.h"
#include <stdexcept>

void ImportState::handle(CHTLParser& parser, Token token) {
    // The [Import] token was consumed by DefaultState.
    // `token` is the first token after `[Import]`.

    std::string category;
    std::string importType;
    std::string name;
    std::string path;
    std::string alias;

    // Check for an optional category like [Custom] or [Template]
    if (token.type == TokenType::KEYWORD_CUSTOM ||
        token.type == TokenType::KEYWORD_TEMPLATE ||
        token.type == TokenType::KEYWORD_ORIGIN) {
        category = token.value;
        token = parser.consume(); // Consume category, move to next token
    }

    // Check for a type like @Chtl or @Element
    if (token.type == TokenType::AT_SIGN) {
        token = parser.consume(); // Consume '@'
        if (token.type != TokenType::IDENTIFIER) {
            throw std::runtime_error("Expected import type identifier after '@'");
        }
        importType = token.value;
        token = parser.consume();
    } else if (category.empty()) {
        // If there's no category, we must have a type starting with @
        throw std::runtime_error("Unexpected token in import statement. Expected category or '@'.");
    }

    // Check for a specific name, for precise imports
    if (!importType.empty() && token.type == TokenType::IDENTIFIER) {
        // This could be the name or the 'from' keyword.
        // Chtl, Html, etc. are file-level imports and won't have a name before 'from'.
        if (token.value != "from") {
            name = token.value;
            token = parser.consume();
        }
    }

    // Now we must see 'from'
    if (token.type != TokenType::KEYWORD_FROM) {
        throw std::runtime_error("Expected 'from' keyword in import statement, got '" + token.value + "'");
    }

    // And then the path
    token = parser.consume();
    if (token.type != TokenType::STRING_LITERAL) {
        throw std::runtime_error("Expected file path string literal in import statement");
    }
    path = token.value;

    // And then the optional 'as' alias
    if (parser.peek().type == TokenType::KEYWORD_AS) {
        parser.consume(); // consume 'as'
        Token aliasToken = parser.consume();
        if (aliasToken.type != TokenType::IDENTIFIER) {
            throw std::runtime_error("Expected alias identifier after 'as'");
        }
        alias = aliasToken.value;
    } else {
        // 'as' is mandatory for non-CHTL file-level imports
        if (category.empty() && importType != "Chtl") {
             throw std::runtime_error("The 'as' keyword is mandatory for importing " + importType + " files.");
        }
    }

    // And then the optional semicolon
    if (parser.peek().type == TokenType::SEMICOLON) {
        parser.consume();
    }

    auto importNode = std::make_unique<ImportNode>(path, alias, category, importType, name);
    parser.addNode(std::move(importNode));

    parser.setState(std::make_unique<DefaultState>());
}