#pragma once

#include "CHTLLexer/Token.h"
#include "CHTLNode/BaseNode.h"
#include "CHTLState/StateMachine.h"
#include "CHTLContext/Context.h" // <-- New include
#include <vector>
#include <memory>

namespace CHTL {

class Parser {
public:
    // The parser now requires a shared context to store compilation state.
    explicit Parser(std::vector<Token> tokens, std::shared_ptr<Context> context);

    // Main entry point to start parsing. Returns the root of the AST.
    std::shared_ptr<BaseNode> parse();

    // --- Public members to be accessed by State and Strategy objects ---

    // The state machine that drives the parser.
    StateMachine stateMachine;

    // The shared context for the compilation unit.
    std::shared_ptr<Context> context;

    // The list of tokens from the lexer.
    const std::vector<Token>& getTokens() const { return tokens; }

    // --- Parser utility methods that states can use ---

    bool isAtEnd();
    Token advance();
    Token peek();
    Token previous();

    // Checks if the current token is of the given type without consuming it.
    bool check(TokenType type);

    // Consumes the current token if it matches any of the given types.
    bool match(const std::vector<TokenType>& types);

private:
    std::vector<Token> tokens;
    size_t current = 0; // Index of the next token to be consumed.

    // For error recovery
    void synchronize();
};

} // namespace CHTL
