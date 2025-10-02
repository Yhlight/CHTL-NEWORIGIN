#include "CHTLParser.h"
#include "../CHTLState/DefaultState.h"
#include <string>

CHTLParser::CHTLParser(const std::string& input) : lexer(input) {
    currentState = std::make_unique<DefaultState>();
}

void CHTLParser::parse() {
    Token token = getNextToken();
    while (token.type != TokenType::END_OF_FILE) {
        currentState->handle(*this, token);
        token = getNextToken();
    }
}

void CHTLParser::setState(std::unique_ptr<CHTLState> newState) {
    currentState = std::move(newState);
}

Token CHTLParser::getNextToken() {
    return lexer.getNextToken();
}