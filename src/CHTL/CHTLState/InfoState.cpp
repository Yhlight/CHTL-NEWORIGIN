#include "InfoState.h"
#include "StatementState.h"
#include "../CHTLParser/Parser.h"
#include "../CHTLNode/InfoNode.h"
#include <stdexcept>

std::unique_ptr<BaseNode> InfoState::handle(Parser& parser) {
    auto infoNode = std::make_unique<InfoNode>();

    parser.expectToken(TokenType::OpenBrace);

    while (parser.currentToken.type != TokenType::CloseBrace) {
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

    // After parsing the info block, we return to the StatementState
    parser.setState(std::make_unique<StatementState>());

    // The InfoNode is not returned directly, it will be stored in the parser.
    parser.infoNode = std::move(infoNode);

    return nullptr;
}