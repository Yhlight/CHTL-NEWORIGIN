#include "Lexer.h"
#include <cctype>
#include <stdexcept>

namespace CHTL
{

Lexer::Lexer(const std::string& source) : m_source(source) {}

std::vector<Token> Lexer::Tokenize()
{
    std::vector<Token> tokens;
    while (!IsAtEnd())
    {
        Token token = GetNextToken();
        tokens.push_back(token);
        if (token.type == TokenType::EndOfFile)
        {
            break;
        }
    }
    return tokens;
}

Token Lexer::GetNextToken()
{
    SkipWhitespaceAndComments();

    if (IsAtEnd())
    {
        return {TokenType::EndOfFile, "", m_line, m_column};
    }

    char c = Peek();

    if (std::isalpha(c) || c == '_')
    {
        return HandleIdentifier();
    }

    if (std::isdigit(c))
    {
        return HandleNumberLiteral();
    }

    if (c == '"' || c == '\'')
    {
        return HandleStringLiteral(c);
    }

    return HandleSymbol();
}

void Lexer::SkipWhitespaceAndComments()
{
    while (!IsAtEnd())
    {
        char c = Peek();
        if (std::isspace(c))
        {
            if (c == '\n')
            {
                m_line++;
                m_column = 1;
            }
            else
            {
                m_column++;
            }
            Advance();
        }
        else if (c == '/')
        {
            if (m_current + 1 < m_source.length())
            {
                if (m_source[m_current + 1] == '/') // Line comment
                {
                    while (!IsAtEnd() && Peek() != '\n')
                    {
                        Advance();
                    }
                }
                else if (m_source[m_current + 1] == '*') // Block comment
                {
                    Advance(); // consume /
                    Advance(); // consume *
                    while (!IsAtEnd())
                    {
                        if (Peek() == '*' && m_current + 1 < m_source.length() && m_source[m_current + 1] == '/')
                        {
                            Advance(); // consume *
                            Advance(); // consume /
                            break;
                        }
                        if (Peek() == '\n')
                        {
                            m_line++;
                            m_column = 1;
                        }
                        Advance();
                    }
                }
                else
                {
                    break; // Not a comment
                }
            }
            else
            {
                break; // Not a comment
            }
        }
        else if (c == '#')
        {
            while (!IsAtEnd() && Peek() != '\n')
            {
                Advance();
            }
        }
        else
        {
            break;
        }
    }
}

Token Lexer::HandleIdentifier()
{
    size_t start = m_current;
    size_t start_col = m_column;
    // CHTL allows '-' in identifiers (e.g. for CSS properties)
    while (!IsAtEnd() && (std::isalnum(Peek()) || Peek() == '_' || Peek() == '-'))
    {
        Advance();
    }
    std::string value = m_source.substr(start, m_current - start);

    // For now, we'll treat all text as identifiers or unquoted literals.
    // The parser will be responsible for distinguishing them based on context.
    return {TokenType::Identifier, value, m_line, start_col};
}

Token Lexer::HandleNumberLiteral()
{
    size_t start = m_current;
    size_t start_col = m_column;
    while (!IsAtEnd() && std::isdigit(Peek()))
    {
        Advance();
    }

    if (Peek() == '.' && m_current + 1 < m_source.length() && std::isdigit(m_source[m_current + 1]))
    {
        Advance(); // consume '.'
        while (!IsAtEnd() && std::isdigit(Peek()))
        {
            Advance();
        }
    }

    std::string value = m_source.substr(start, m_current - start);
    return {TokenType::NumberLiteral, value, m_line, start_col};
}

Token Lexer::HandleStringLiteral(char quoteType)
{
    size_t start_col = m_column;
    Advance(); // consume opening quote
    size_t start = m_current;
    while (!IsAtEnd() && Peek() != quoteType)
    {
        if (Peek() == '\n')
        {
            // CHTL spec doesn't specify multi-line strings, assume error for now
            // or just advance and let the parser handle it.
            m_line++;
            m_column = 1;
        }
        Advance();
    }

    if (IsAtEnd())
    {
        return {TokenType::Unexpected, "Unterminated string", m_line, start_col};
    }

    std::string value = m_source.substr(start, m_current - start);
    Advance(); // consume closing quote
    return {TokenType::StringLiteral, value, m_line, start_col};
}

Token Lexer::HandleSymbol()
{
    size_t start_col = m_column;
    char c = Advance();
    switch (c)
    {
        case '{': return {TokenType::OpenBrace, "{", m_line, start_col};
        case '}': return {TokenType::CloseBrace, "}", m_line, start_col};
        case '(': return {TokenType::OpenParen, "(", m_line, start_col};
        case ')': return {TokenType::CloseParen, ")", m_line, start_col};
        case '[': return {TokenType::OpenBracket, "[", m_line, start_col};
        case ']': return {TokenType::CloseBracket, "]", m_line, start_col};
        case ';': return {TokenType::Semicolon, ";", m_line, start_col};
        case ':': return {TokenType::Colon, ":", m_line, start_col};
        case '=': return {TokenType::Equals, "=", m_line, start_col};
        case ',': return {TokenType::Comma, ",", m_line, start_col};
        case '.': return {TokenType::Dot, ".", m_line, start_col};
        case '@': return {TokenType::At, "@", m_line, start_col};
        case '+': return {TokenType::Plus, "+", m_line, start_col};
        case '-': return {TokenType::Minus, "-", m_line, start_col};
        case '/': return {TokenType::Slash, "/", m_line, start_col};
        case '%': return {TokenType::Percent, "%", m_line, start_col};
        case '>': return {TokenType::GreaterThan, ">", m_line, start_col};
        case '<': return {TokenType::LessThan, "<", m_line, start_col};
        case '?': return {TokenType::QuestionMark, "?", m_line, start_col};
        case '*':
            if (Peek() == '*') { Advance(); return {TokenType::DoubleAsterisk, "**", m_line, start_col}; }
            return {TokenType::Asterisk, "*", m_line, start_col};
        case '&':
            if (Peek() == '&') { Advance(); return {TokenType::DoubleAmpersand, "&&", m_line, start_col}; }
            return {TokenType::Ampersand, "&", m_line, start_col};
        case '|':
            if (Peek() == '|') { Advance(); return {TokenType::DoublePipe, "||", m_line, start_col}; }
            // Fallthrough to unexpected, as single '|' is not in the spec
        default:
            return {TokenType::Unexpected, std::string(1, c), m_line, start_col};
    }
}

char Lexer::Peek()
{
    if (IsAtEnd()) return '\0';
    return m_source[m_current];
}

char Lexer::Advance()
{
    m_column++;
    return m_source[m_current++];
}

bool Lexer::IsAtEnd()
{
    return m_current >= m_source.length();
}

} // namespace CHTL
