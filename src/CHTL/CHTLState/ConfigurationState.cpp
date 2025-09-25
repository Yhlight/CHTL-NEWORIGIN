#include "ConfigurationState.h"
#include "../CHTLParser/Parser.h"
#include <stdexcept>

std::unique_ptr<BaseNode> ConfigurationState::handle(Parser& parser) {
    // The dispatcher in StatementState has already consumed `[` and the keyword.
    // We just need to consume the closing bracket.
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

            std::vector<std::string> aliases;
            if (parser.currentToken.type == TokenType::OpenBracket) {
                // Handle group alias: [alias1, alias2]
                parser.advanceTokens(); // consume '['
                while (parser.currentToken.type != TokenType::CloseBracket && parser.currentToken.type != TokenType::EndOfFile) {
                    aliases.push_back(parser.currentToken.value);
                    parser.advanceTokens();
                    if (parser.currentToken.type == TokenType::Comma) {
                        parser.advanceTokens(); // consume ','
                    } else {
                        break; // No comma, end of list
                    }
                }
                parser.expectToken(TokenType::CloseBracket);
            } else {
                // Handle single alias
                aliases.push_back(parser.currentToken.value);
                parser.advanceTokens();
            }

            parser.configManager.keywordMap[key] = aliases;
            parser.expectToken(TokenType::Semicolon);
        } else {
            throw std::runtime_error("Unexpected token in [Name] block: " + parser.currentToken.value);
        }
    }
    parser.expectToken(TokenType::CloseBrace);
}
