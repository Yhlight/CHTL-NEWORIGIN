#pragma once

#include "CHTLLexer/Token.h"
#include "CHTLNode/BaseNode.h"
#include "CHTLState/StateMachine.h"
#include "CHTLContext/Context.h"
#include <vector>
#include <memory>
#include <stack> // <-- New include

namespace CHTL {

class Parser {
public:
    explicit Parser(std::vector<Token> tokens, std::shared_ptr<Context> context);
    std::shared_ptr<BaseNode> parse();

    // --- Public members to be accessed by State and Strategy objects ---

    StateMachine stateMachine;
    std::shared_ptr<Context> context;

    // AST management: The states will build the tree using these.
    std::shared_ptr<BaseNode> root;
    std::stack<std::shared_ptr<BaseNode>> nodeStack;

    const std::vector<Token>& getTokens() const { return tokens; }

    // --- Parser utility methods that states can use ---
    bool isAtEnd();
    Token advance();
    Token peek();
    Token peekNext();
    Token previous();
    bool check(TokenType type);
    bool match(const std::vector<TokenType>& types);

private:
    std::vector<Token> tokens;
    size_t current = 0;

    void synchronize();
};

} // namespace CHTL
