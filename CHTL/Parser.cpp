#include "Parser.h"
#include "CHTLState/GlobalState.h"
#include <stdexcept>

namespace CHTL
{
    Parser::Parser(const std::vector<Token>& tokens) : m_tokens(tokens) {}

    std::vector<std::unique_ptr<AST::BaseNode>> Parser::Parse()
    {
        m_states.push_back(std::make_unique<State::GlobalState>());

        while (!m_states.empty() && !IsAtEnd())
        {
            m_states.back()->Handle(*this);
        }

        return std::move(m_rootNodes);
    }

    void Parser::PushState(std::unique_ptr<State::IState> newState)
    {
        m_states.push_back(std::move(newState));
    }

    void Parser::PopState()
    {
        if (!m_states.empty())
        {
            m_states.pop_back();
        }
    }

    void Parser::AddRootNode(std::unique_ptr<AST::BaseNode> node)
    {
        m_rootNodes.push_back(std::move(node));
    }

    // --- Token methods ---
    const Token& Parser::Peek(size_t offset)
    {
        if (m_current + offset >= m_tokens.size())
        {
            return m_tokens.back();
        }
        return m_tokens[m_current + offset];
    }

    Token Parser::Consume()
    {
        if (!IsAtEnd())
        {
            return m_tokens[m_current++];
        }
        return m_tokens.back();
    }

    bool Parser::IsAtEnd()
    {
        return m_current >= m_tokens.size() || Peek().type == TokenType::EndOfFile;
    }

    bool Parser::Match(TokenType type)
    {
        if (IsAtEnd()) return false;
        if (Peek().type == type)
        {
            Consume();
            return true;
        }
        return false;
    }

    void Parser::Expect(TokenType type, const std::string& errorMessage)
    {
        if (IsAtEnd() || Peek().type != type)
        {
            throw std::runtime_error(errorMessage);
        }
        Consume();
    }

} // namespace CHTL
