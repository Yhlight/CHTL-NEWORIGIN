#include "UseState.h"
#include "StatementState.h"
#include "../CHTLParser/Parser.h" // Include Parser.h for access to the Parser class

#include <stdexcept>

std::unique_ptr<BaseNode> UseState::handle(Parser& parser) {
    parser.expectToken(TokenType::Use); // Consume 'use'
    if (parser.currentToken.type == TokenType::Identifier && parser.currentToken.value == "html5") {
        parser.advanceTokens(); // Consume 'html5'
        parser.expectToken(TokenType::Semicolon);
        parser.outputHtml5Doctype = true;
    } else {
        throw std::runtime_error("Unexpected token after 'use': " + parser.currentToken.value);
    }

    parser.setState(std::make_unique<StatementState>());
    return nullptr; // Directives don't produce nodes
}