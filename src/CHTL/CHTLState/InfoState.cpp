#include "InfoState.h"
#include "StatementState.h"
#include "../CHTLParser/Parser.h"
#include "../CHTLNode/InfoNode.h"
#include <stdexcept>
#include <sstream>

std::unique_ptr<BaseNode> InfoState::handle(Parser& parser) {
    // This state is entered after '[Info]' has been consumed.
    // It is responsible for parsing the content within the braces {}.
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

            // Check for string literal value
            if (parser.currentToken.type != TokenType::String) {
                throw std::runtime_error("Expected a string literal for the value of the info property.");
            }
            infoNode->metadata[key] = parser.currentToken.value;
            parser.advanceTokens();

            // Properties should be separated by semicolons
            if (parser.currentToken.type == TokenType::Semicolon) {
                parser.advanceTokens();
            }
        } else {
            // Handle the case of an empty info block {} correctly
            if (parser.currentToken.type == TokenType::CloseBrace) {
                break;
            }
            throw std::runtime_error("Unexpected token in [Info] block: " + parser.currentToken.value);
        }
    }

    parser.expectToken(TokenType::CloseBrace);

    // Store the parsed info node in the parser context.
    // It's crucial that a file has only one [Info] block.
    if (parser.infoNode) {
        throw std::runtime_error("Multiple [Info] blocks are not allowed in a single file.");
    }
    parser.infoNode = std::move(infoNode);

    // After parsing, return to the StatementState to handle subsequent tokens.
    parser.setState(std::make_unique<StatementState>());

    // This state produces no node for the AST itself, it just populates parser context.
    return nullptr;
}