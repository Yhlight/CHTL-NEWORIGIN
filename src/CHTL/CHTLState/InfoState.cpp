#include "InfoState.h"
#include "StatementState.h"
#include "../CHTLParser/Parser.h"
#include "../CHTLNode/InfoNode.h"
#include <stdexcept>
#include <sstream>

void parseExportBlock(Parser& parser, InfoNode& infoNode) {
    parser.expectToken(TokenType::OpenBracket);
    if (parser.currentToken.value != "Export") {
        throw std::runtime_error("Expected [Export] block after [Info] block.");
    }
    parser.expectToken(TokenType::Identifier);
    parser.expectToken(TokenType::CloseBracket);
    parser.expectToken(TokenType::OpenBrace);

    while (parser.currentToken.type != TokenType::CloseBrace) {
        std::stringstream key_builder;
        if (parser.currentToken.type == TokenType::OpenBracket) {
            key_builder << parser.currentToken.value; // [
            parser.advanceTokens();
            key_builder << parser.currentToken.value; // Custom or Template
            parser.advanceTokens();
            key_builder << parser.currentToken.value; // ]
            parser.expectToken(TokenType::CloseBracket);
            key_builder << " ";
        }

        key_builder << parser.currentToken.value; // @
        parser.expectToken(TokenType::At);
        key_builder << parser.currentToken.value; // Type (e.g., Element)
        parser.expectToken(TokenType::Identifier);

        std::string key = key_builder.str();
        std::vector<std::string> symbols;
        while (parser.currentToken.type != TokenType::Semicolon) {
            if (parser.currentToken.type != TokenType::Identifier) {
                throw std::runtime_error("Expected identifier in export list.");
            }
            symbols.push_back(parser.currentToken.value);
            parser.advanceTokens();
            if (parser.currentToken.type == TokenType::Comma) {
                parser.advanceTokens();
            }
        }
        infoNode.exports[key] = symbols;
        parser.expectToken(TokenType::Semicolon);
    }

    parser.expectToken(TokenType::CloseBrace);
}

std::unique_ptr<BaseNode> InfoState::handle(Parser& parser) {
    auto infoNode = std::make_unique<InfoNode>();

    parser.expectToken(TokenType::OpenBrace);

    while (parser.currentToken.type != TokenType::CloseBrace) {
        if (parser.currentToken.type == TokenType::OpenBracket) {
             // If we find an [Export] block, parse it and break from the [Info] loop.
             if (parser.peekToken.value == "Export") {
                 parseExportBlock(parser, *infoNode);
                 continue;
             }
        }

        if (parser.currentToken.type == TokenType::Identifier) {
            std::string key = parser.currentToken.value;
            parser.advanceTokens();
            if (parser.currentToken.type != TokenType::Colon && parser.currentToken.type != TokenType::Equals) {
                throw std::runtime_error("Expected ':' or '=' after info key '" + key + "'.");
            }
            parser.advanceTokens(); // Consume ':' or '='
            if (parser.currentToken.type != TokenType::String) {
                throw std::runtime_error("Expected a string literal for the value of the info property.");
            }
            infoNode->metadata[key] = parser.currentToken.value;
            parser.advanceTokens();
            parser.expectToken(TokenType::Semicolon);
        } else {
            throw std::runtime_error("Unexpected token in [Info] block: " + parser.currentToken.value);
        }
    }

    parser.expectToken(TokenType::CloseBrace);

    // After parsing, return to the StatementState
    parser.setState(std::make_unique<StatementState>());

    // The InfoNode is not returned directly, it will be stored in the parser.
    parser.infoNode = std::move(infoNode);

    return nullptr;
}