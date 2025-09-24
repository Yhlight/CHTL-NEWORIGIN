#include "Parser.h"
#include "CHTLState/GlobalScopeState.h"
#include "CHTLContext/Context.h"
#include <utility>

namespace CHTL {

Parser::Parser(std::vector<Token> tokens, std::shared_ptr<Context> context)
    : tokens(std::move(tokens)), context(std::move(context)) {}

std::shared_ptr<BaseNode> Parser::parse() {
    // The main parsing loop is driven by the state machine.
    // We initialize it to the GlobalScopeState.
    stateMachine.changeState(std::make_unique<GlobalScopeState>(this));

    while (!isAtEnd()) {
        stateMachine.update();
        // In the future, we can add error recovery here.
        // For now, we assume valid syntax.
    }

    // The AST root will be built by the states.
    // For now, we don't have a root node, so we return nullptr.
    return nullptr;
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() {
    return tokens.at(current);
}

Token Parser::previous() {
    return tokens.at(current - 1);
}

Token Parser::advance() {
    if (!isAtEnd()) {
        current++;
    }
    return previous();
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

void Parser::synchronize() {
    // Basic error recovery: advance tokens until we find a likely synchronization point.
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;

        switch (peek().type) {
            case TokenType::BRACKETED_KEYWORD:
            case TokenType::KEYWORD_TEXT:
            case TokenType::KEYWORD_STYLE:
            case TokenType::IDENTIFIER: // Likely start of an element
                return;
            default:
                break;
        }
        advance();
    }
}

} // namespace CHTL
