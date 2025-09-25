#include "ConfigurationState.h"
#include "StatementState.h" // For transitioning back
#include "../CHTLParser/Parser.h"
#include <stdexcept>

std::unique_ptr<BaseNode> ConfigurationState::handle(Parser& parser) {
    // StatementState now consumes the entire `[Configuration]` decorator.
    std::string configName = "_default";
    bool isNewConfig = false;

    // Check for a named configuration: @Config <name>
    if (parser.currentToken.type == TokenType::At) {
        parser.advanceTokens(); // Consume '@'
        if (parser.currentToken.value != "Config") {
            throw std::runtime_error("Expected 'Config' after '@' for named configuration.");
        }
        parser.advanceTokens(); // Consume 'Config'
        configName = parser.currentToken.value;
        parser.expectToken(TokenType::Identifier);
        isNewConfig = true;
    }

    // Get the configuration set we'll be modifying.
    auto configSet = std::make_unique<ConfigSet>();
    if (!isNewConfig) {
        // If we are modifying the default config, start with a copy of the existing one.
        *configSet = *parser.configManager.getActiveConfig();
    }
    ConfigSet* currentConfig = configSet.get();

    parser.expectToken(TokenType::OpenBrace);

    while (parser.currentToken.type != TokenType::CloseBrace && parser.currentToken.type != TokenType::EndOfFile) {
        if (parser.currentToken.type == TokenType::OpenBracket && parser.peekToken.value == "Name") {
            parseNameBlock(parser, *currentConfig);
        } else if (parser.currentToken.type == TokenType::Identifier) {
            std::string key = parser.currentToken.value;
            parser.advanceTokens();
            parser.expectToken(TokenType::Equals);
            std::string value = parser.currentToken.value;

            if (key == "DEBUG_MODE") {
                currentConfig->debugMode = (value == "true");
            } else if (key == "INDEX_INITIAL_COUNT") {
                currentConfig->indexInitialCount = std::stoi(value);
            } else if (key == "DISABLE_STYLE_AUTO_ADD_CLASS") {
                currentConfig->disableStyleAutoAddClass = (value == "true");
            } else if (key == "DISABLE_STYLE_AUTO_ADD_ID") {
                currentConfig->disableStyleAutoAddId = (value == "true");
            }

            parser.advanceTokens();
            parser.expectToken(TokenType::Semicolon);
        } else {
            throw std::runtime_error("Unexpected token in [Configuration] block: " + parser.currentToken.value);
        }
    }

    parser.expectToken(TokenType::CloseBrace);

    // Add the new or updated configuration to the manager.
    parser.configManager.addConfig(configName, std::move(configSet));

    // After parsing, we should be back in the general statement context.
    parser.setState(std::make_unique<StatementState>());
    return nullptr;
}

void ConfigurationState::parseNameBlock(Parser& parser, ConfigSet& configSet) {
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
                parser.advanceTokens(); // consume '['
                while (parser.currentToken.type != TokenType::CloseBracket && parser.currentToken.type != TokenType::EndOfFile) {
                    aliases.push_back(parser.currentToken.value);
                    parser.advanceTokens();
                    if (parser.currentToken.type == TokenType::Comma) {
                        parser.advanceTokens();
                    } else {
                        break;
                    }
                }
                parser.expectToken(TokenType::CloseBracket);
            } else {
                aliases.push_back(parser.currentToken.value);
                parser.advanceTokens();
            }

            configSet.keywordMap[key] = aliases;
            parser.expectToken(TokenType::Semicolon);
        } else {
            throw std::runtime_error("Unexpected token in [Name] block: " + parser.currentToken.value);
        }
    }
    parser.expectToken(TokenType::CloseBrace);
}