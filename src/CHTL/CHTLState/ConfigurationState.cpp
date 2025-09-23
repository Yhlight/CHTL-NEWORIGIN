#include "ConfigurationState.h"
#include "../CHTLParser/Parser.h"
#include <stdexcept>

std::unique_ptr<BaseNode> ConfigurationState::handle(Parser& parser) {
    parser.expectToken(TokenType::OpenBracket);
    parser.expectToken(TokenType::Configuration);
    parser.expectToken(TokenType::CloseBracket);

    // Optional: handle named configuration blocks, e.g., [Configuration] @MyConfig
    // For now, we only parse unnamed, active configs.
    if (parser.currentToken.type == TokenType::At) {
        // For now, just consume the name and ignore it.
        parser.advanceTokens(); // consume @
        parser.advanceTokens(); // consume name
    }

    parser.expectToken(TokenType::OpenBrace);

    while (parser.currentToken.type != TokenType::CloseBrace && parser.currentToken.type != TokenType::EndOfFile) {
        if (parser.currentToken.type == TokenType::OpenBracket && parser.peekToken.value == "Name") {
            parseNameBlock(parser);
        } else if (parser.currentToken.type == TokenType::Identifier) {
            // Handle simple key-value pairs
            std::string key = parser.currentToken.value;
            parser.advanceTokens();
            parser.expectToken(TokenType::Equals);

            std::string value = parser.currentToken.value;
            // For now, we assume simple boolean or integer values.
            // A more robust implementation would parse different value types.
            if (key == "DEBUG_MODE") {
                parser.configManager.debugMode = (value == "true");
            } else if (key == "INDEX_INITIAL_COUNT") {
                parser.configManager.indexInitialCount = std::stoi(value);
            } else if (key == "DISABLE_STYLE_AUTO_ADD_CLASS") {
                parser.configManager.disableStyleAutoAddClass = (value == "true");
            } else if (key == "DISABLE_STYLE_AUTO_ADD_ID") {
                parser.configManager.disableStyleAutoAddId = (value == "true");
            }
            // Add other config flags here...

            parser.advanceTokens();
            parser.expectToken(TokenType::Semicolon);

        } else {
            throw std::runtime_error("Unexpected token in [Configuration] block: " + parser.currentToken.value);
        }
    }

    parser.expectToken(TokenType::CloseBrace);
    return nullptr; // This state does not produce an AST node.
}

void ConfigurationState::parseNameBlock(Parser& parser) {
    parser.expectToken(TokenType::OpenBracket);
    parser.expectToken(TokenType::Identifier); // consume "Name"
    parser.expectToken(TokenType::CloseBracket);
    parser.expectToken(TokenType::OpenBrace);

    while (parser.currentToken.type != TokenType::CloseBrace && parser.currentToken.type != TokenType::EndOfFile) {
        if (parser.currentToken.type == TokenType::Identifier) {
            std::string key = parser.currentToken.value;
            parser.advanceTokens();
            parser.expectToken(TokenType::Equals);

            // For now, we only handle simple string-to-string re-definitions.
            // The spec shows group options like `CUSTOM_STYLE = [@Style, @style]`,
            // which requires more complex value parsing. We'll start simple.
            std::string value = parser.currentToken.value;
            parser.configManager.keywordMap[key] = value;
            parser.advanceTokens();
            parser.expectToken(TokenType::Semicolon);
        } else {
            throw std::runtime_error("Unexpected token in [Name] block: " + parser.currentToken.value);
        }
    }
    parser.expectToken(TokenType::CloseBrace);
}
