#include "Lexer.h"
#include <iostream>

namespace CHTL
{

    Lexer::Lexer(const std::string &source) : source(source) {}

    std::vector<Token> Lexer::tokenize()
    {
        while (!isAtEnd())
        {
            start = current;
            scanToken();
        }

        tokens.push_back({TokenType::TOKEN_EOF, "", line});
        return tokens;
    }

    void Lexer::scanToken()
    {
        char c = advance();
        switch (c)
        {
        // Single-character tokens
        case '{':
            tokens.push_back(makeToken(TokenType::TOKEN_LEFT_BRACE));
            break;
        case '}':
            tokens.push_back(makeToken(TokenType::TOKEN_RIGHT_BRACE));
            break;
        case ':':
            tokens.push_back(makeToken(TokenType::TOKEN_COLON));
            break;
        case '=':
            tokens.push_back(makeToken(TokenType::TOKEN_EQUAL));
            break;
        case ';':
            tokens.push_back(makeToken(TokenType::TOKEN_SEMICOLON));
            break;

        // Comments and special characters
        case '/':
            if (match('/'))
            {
                // A // comment goes until the end of the line.
                while (peek() != '\n' && !isAtEnd())
                    advance();
            }
            else if (match('*'))
            {
                // A /* */ comment.
                while (!(peek() == '*' && peekNext() == '/') && !isAtEnd())
                {
                    if (peek() == '\n')
                        line++;
                    advance();
                }
                if (!isAtEnd())
                {
                    advance(); // consume '*'
                    advance(); // consume '/'
                }
            }
            else
            {
                // NOTE: Division is not a feature of CHTL attributes yet.
                // For now, treat '/' as an unknown character.
                tokens.push_back(makeToken(TokenType::TOKEN_UNKNOWN));
            }
            break;
        case '#':
            handleGeneratorComment();
            break;

        // Whitespace
        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace.
            break;
        case '\n':
            line++;
            break;

        // Quoted Strings
        case '"':
        case '\'':
            handleQuotedString(c);
            break;

        default:
            if (isAlpha(c) || isDigit(c))
            {
                handleIdentifierOrLiteral();
            }
            else
            {
                tokens.push_back(makeToken(TokenType::TOKEN_UNKNOWN));
                std::cerr << "Unexpected character '" << c << "' on line " << line << std::endl;
            }
            break;
        }
    }

    char Lexer::advance()
    {
        current++;
        return source[current - 1];
    }

    bool Lexer::match(char expected)
    {
        if (isAtEnd())
            return false;
        if (source[current] != expected)
            return false;
        current++;
        return true;
    }

    char Lexer::peek() const
    {
        if (isAtEnd())
            return '\0';
        return source[current];
    }

    char Lexer::peekNext() const
    {
        if (current + 1 >= source.length())
            return '\0';
        return source[current + 1];
    }

    bool Lexer::isAtEnd() const
    {
        return current >= source.length();
    }

    bool Lexer::isAlpha(char c) const
    {
        return (c >= 'a' && c <= 'z') ||
               (c >= 'A' && c <= 'Z') ||
               c == '_';
    }

    bool Lexer::isDigit(char c) const
    {
        return c >= '0' && c <= '9';
    }

    bool Lexer::isAlphaNumeric(char c) const
    {
        return isAlpha(c) || isDigit(c);
    }

    void Lexer::handleIdentifierOrLiteral()
    {
        // CHTL identifiers/literals can contain letters, numbers, hyphens, and dots.
        while (isAlphaNumeric(peek()) || peek() == '-' || peek() == '.')
            advance();

        std::string text = source.substr(start, current - start);
        tokens.push_back(makeToken(TokenType::TOKEN_IDENTIFIER, text));
    }

    void Lexer::handleQuotedString(char quote_type)
    {
        while (peek() != quote_type && !isAtEnd())
        {
            if (peek() == '\n')
                line++;
            advance();
        }

        if (isAtEnd())
        {
            std::cerr << "Unterminated string on line " << line << std::endl;
            // Don't add a token, or add an error token. For now, do nothing.
            return;
        }

        // The closing quote.
        advance();

        // Trim the surrounding quotes.
        std::string value = source.substr(start + 1, current - start - 2);
        tokens.push_back(makeToken(TokenType::TOKEN_QUOTED_STRING, value));
    }

    void Lexer::handleGeneratorComment()
    {
        // A # comment goes until the end of the line.
        while (peek() != '\n' && !isAtEnd())
        {
            advance();
        }
        // Don't consume the newline, let the main loop handle it.
        std::string value = source.substr(start, current - start);
        tokens.push_back(makeToken(TokenType::TOKEN_GENERATOR_COMMENT, value));
    }

    Token Lexer::makeToken(TokenType type) const
    {
        return {type, source.substr(start, current - start), line};
    }

    Token Lexer::makeToken(TokenType type, const std::string &lexeme) const
    {
        return {type, lexeme, line};
    }

}
