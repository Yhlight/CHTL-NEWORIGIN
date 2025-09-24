#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "CHTLLexer/Token.h"
#include "CHTLNode/BaseNode.h"
#include <vector>
#include <memory>

#include "CHTLState/IState.h"

namespace CHTL
{
    class Parser
    {
    public:
        Parser(const std::vector<Token>& tokens);

        std::vector<std::unique_ptr<AST::BaseNode>> Parse();

        // Token stream methods
        const Token& Peek(size_t offset = 0);
        Token Consume();
        bool IsAtEnd();
        bool Match(TokenType type);
        void Expect(TokenType type, const std::string& errorMessage);

        // State management
        void PushState(std::unique_ptr<State::IState> newState);
        void PopState();

        // Root node management
        void AddRootNode(std::unique_ptr<AST::BaseNode> node);

    private:
        const std::vector<Token>& m_tokens;
        size_t m_current = 0;

        std::vector<std::unique_ptr<State::IState>> m_states;
        std::vector<std::unique_ptr<AST::BaseNode>> m_rootNodes;
    };
}

#endif // CHTL_PARSER_H
