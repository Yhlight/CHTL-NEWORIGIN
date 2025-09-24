#include "Parser.h"
#include "../CHTLContext/Context.h"
#include "../CHTLState/ParsingState.h"

namespace CHTL {

Parser::Parser(Context& context, Loader& loader) : context(context), loader(loader) {}

void Parser::Parse(std::vector<Token> tokens) {
    this->tokens = std::move(tokens);
    this->currentTokenIndex = 0;

    // The context should have its state set by the loader/main
    // I will assume it's already set to a ParsingState.

    while (!this->IsAtEnd()) {
        context.Request(*this);
    }
}

const Token& Parser::CurrentToken() {
    if (currentTokenIndex >= tokens.size()) {
        static Token eof_token = {TokenType::END_OF_FILE, ""};
        return eof_token;
    }
    return tokens[currentTokenIndex];
}

const Token& Parser::PeekNextToken() {
    if (currentTokenIndex + 1 >= tokens.size()) {
        static Token eof_token = {TokenType::END_OF_FILE, ""};
        return eof_token;
    }
    return tokens[currentTokenIndex + 1];
}

const Token& Parser::PeekTokenAt(size_t offset) const {
    if (currentTokenIndex + offset >= tokens.size()) {
        static Token eof_token = {TokenType::END_OF_FILE, ""};
        return eof_token;
    }
    return tokens[currentTokenIndex + offset];
}

void Parser::ConsumeToken() {
    if (!IsAtEnd()) {
        currentTokenIndex++;
    }
}

bool Parser::IsAtEnd() {
    return tokens.empty() || tokens[currentTokenIndex].type == TokenType::END_OF_FILE;
}

void Parser::InjectTokens(const std::vector<Token>& newTokens) {
    if (newTokens.empty()) return;
    tokens.insert(tokens.begin() + currentTokenIndex, newTokens.begin(), newTokens.end());
}

} // namespace CHTL
