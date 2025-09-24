#include "Parser.h"
#include "CHTLState/GlobalScopeState.h"
#include "CHTLContext/Context.h"
#include "CHTLNode/ElementNode.h" // For creating a dummy root
#include <utility>

namespace CHTL {

Parser::Parser(std::vector<Token> tokens, std::shared_ptr<Context> context)
    : tokens(std::move(tokens)), context(std::move(context)) {}

std::shared_ptr<BaseNode> Parser::parse() {
    // 1. Create a root node to hold the entire document. This makes handling
    //    multiple top-level elements easier.
    root = std::make_shared<ElementNode>("__ROOT__");

    // 2. Push the root node onto the stack to act as the initial parent.
    nodeStack.push(root);

    // 3. Start the state machine in the global scope.
    stateMachine.changeState(std::make_unique<GlobalScopeState>(this));

    // 4. Loop until all tokens are consumed.
    while (!isAtEnd()) {
        stateMachine.update();
    }

    // 5. Return the completed AST.
    return root;
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() {
    return tokens.at(current);
}

Token Parser::peekNext() {
    if (current + 1 >= tokens.size()) {
        return tokens.back();
    }
    return tokens.at(current + 1);
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
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;
        switch (peek().type) {
            case TokenType::BRACKETED_KEYWORD:
            case TokenType::KEYWORD_TEXT:
            case TokenType::KEYWORD_STYLE:
            case TokenType::IDENTIFIER:
                return;
            default:
                break;
        }
        advance();
    }
}

} // namespace CHTL
