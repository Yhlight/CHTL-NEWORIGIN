#include "Lexer.h"
#include <cctype>

namespace CHTL {

Lexer::Lexer(std::string input)
    : m_input(std::move(input)), m_position(0), m_readPosition(0), m_char(0) {
    readChar();
}

void Lexer::readChar() {
    if (m_readPosition >= m_input.length()) {
        m_char = 0; // EOF
    } else {
        m_char = m_input[m_readPosition];
    }
    m_position = m_readPosition;
    m_readPosition += 1;
}

char Lexer::peekChar() const {
    if (m_readPosition >= m_input.length()) {
        return 0;
    }
    return m_input[m_readPosition];
}

void Lexer::skipWhitespace() {
    while (isspace(m_char)) {
        readChar();
    }
}

void Lexer::skipSingleLineComment() {
    while (m_char != '\n' && m_char != 0) {
        readChar();
    }
}

void Lexer::skipMultiLineComment() {
    while (true) {
        if (m_char == 0) { // EOF
            break;
        }
        if (m_char == '*' && peekChar() == '/') {
            readChar(); // Consume '*'
            readChar(); // Consume '/'
            break;
        }
        readChar();
    }
}

Token Lexer::readString(char quote) {
    size_t startPosition = m_position + 1;
    do {
        readChar();
    } while (m_char != quote && m_char != 0);

    std::string literal = m_input.substr(startPosition, m_position - startPosition);
    readChar(); // Consume the closing quote
    return {TokenType::String, literal};
}

Token Lexer::NextToken() {
    while (true) {
        skipWhitespace();

        if (m_char == '/') {
            if (peekChar() == '/') {
                readChar();
                readChar();
                skipSingleLineComment();
                continue;
            }
            if (peekChar() == '*') {
                readChar();
                readChar();
                skipMultiLineComment();
                continue;
            }
        }
        break;
    }

    Token tok(TokenType::Illegal, std::string(1, m_char));

    switch (m_char) {
        case '#':
            {
                readChar();
                if (m_char != ' ') {
                    return {TokenType::Illegal, "#" + std::string(1, m_char)};
                }
                readChar();
                auto start = m_position;
                while(m_char != '\n' && m_char != 0) {
                    readChar();
                }
                tok = Token(TokenType::GeneratorComment, m_input.substr(start, m_position - start));
                break;
            }
        case '{':
            tok = Token(TokenType::LBrace, "{");
            break;
        case '}':
            tok = Token(TokenType::RBrace, "}");
            break;
        case ':':
            tok = Token(TokenType::Colon, ":");
            break;
        case ';':
            tok = Token(TokenType::Semicolon, ";");
            break;
        case '=':
            tok = Token(TokenType::Equals, "=");
            break;
        case '"':
        case '\'':
            return readString(m_char);
        case 0:
            tok = Token(TokenType::Eof, "");
            break;
        default:
            {
                size_t start = m_position;
                while (m_char != 0 && !isspace(m_char) && m_char != '{' && m_char != '}' && m_char != ':' && m_char != ';') {
                    readChar();
                }
                std::string literal = m_input.substr(start, m_position - start);
                return {TokenType::Identifier, literal};
            }
    }

    readChar();
    return tok;
}

} // CHTL
