#include "CHTLLexer.h"
#include <cctype>
#include <map>

namespace CHTL {

// Map for simple keyword lookup.
static const std::map<std::string, TokenType> KEYWORDS = {
    {"style", TokenType::KEYWORD_STYLE},
    {"script", TokenType::KEYWORD_SCRIPT},
    {"from", TokenType::KEYWORD_FROM},
    {"as", TokenType::KEYWORD_AS},
};

CHTLLexer::CHTLLexer(const std::string& source) : m_source(source) {}

char CHTLLexer::currentChar() const {
    if (m_position >= m_source.length()) {
        return '\0'; // End of file character
    }
    return m_source[m_position];
}

void CHTLLexer::advance() {
    if (m_position < m_source.length()) {
        if (currentChar() == '\n') {
            m_line++;
            m_column = 1;
        } else {
            m_column++;
        }
        m_position++;
    }
}

Token CHTLLexer::makeToken(TokenType type, const std::string& value) {
    return {type, value, m_line, m_column - (int)value.length()};
}

Token CHTLLexer::makeToken(TokenType type, char value) {
    return {type, std::string(1, value), m_line, m_column};
}

void CHTLLexer::skipWhitespace() {
    while (currentChar() != '\0' && isspace(currentChar())) {
        advance();
    }
}

void CHTLLexer::skipLineComment() {
    while (currentChar() != '\0' && currentChar() != '\n') {
        advance();
    }
}

void CHTLLexer::skipBlockComment() {
    advance(); // Consume '*'
    while (m_position < m_source.length()) {
        if (currentChar() == '*' && m_position + 1 < m_source.length() && m_source[m_position + 1] == '/') {
            advance(); advance(); // Consume '*/'
            return;
        }
        advance();
    }
}

Token CHTLLexer::identifier() {
    std::string value;
    int start_col = m_column;
    // Allow hyphens in identifiers for CSS properties like font-size.
    while (isalnum(currentChar()) || currentChar() == '_' || currentChar() == '-') {
        value += currentChar();
        advance();
    }

    auto it = KEYWORDS.find(value);
    if (it != KEYWORDS.end()) {
        return {it->second, value, m_line, start_col};
    }

    return {TokenType::IDENTIFIER, value, m_line, start_col};
}

Token CHTLLexer::stringLiteral() {
    std::string value;
    int start_col = m_column;
    char quote_type = currentChar();
    advance(); // Consume opening quote

    while (currentChar() != quote_type && currentChar() != '\0') {
        value += currentChar();
        advance();
    }

    if (currentChar() == quote_type) {
        advance(); // Consume closing quote
    }

    return {TokenType::STRING_LITERAL, value, m_line, start_col};
}

Token CHTLLexer::getNextToken() {
    while (currentChar() != '\0') {
        if (isspace(currentChar())) {
            skipWhitespace();
            continue;
        }

        if (currentChar() == '/') {
            if (m_position + 1 < m_source.length()) {
                if (m_source[m_position + 1] == '/') {
                    advance(); advance(); skipLineComment();
                    continue;
                }
                if (m_source[m_position + 1] == '*') {
                    advance(); advance(); skipBlockComment();
                    continue;
                }
            }
        }

        if (currentChar() == '#') {
            int start_col = m_column;
            advance(); // consume '#'
            if (isspace(currentChar())) {
                advance(); // consume space
                std::string value;
                while (currentChar() != '\0' && currentChar() != '\n') {
                    value += currentChar();
                    advance();
                }
                size_t end = value.find_last_not_of(" \t\r");
                if (std::string::npos != end) value = value.substr(0, end + 1);
                return {TokenType::GENERATOR_COMMENT, value, m_line, start_col};
            }
            return {TokenType::HASH, "#", m_line, start_col};
        }

        if (isalpha(currentChar()) || currentChar() == '_') return identifier();
        if (isdigit(currentChar())) return identifier(); // Treat as unquoted literal for now
        if (currentChar() == '"' || currentChar() == '\'') return stringLiteral();

        Token token;
        switch (currentChar()) {
            case '{': token = makeToken(TokenType::LEFT_BRACE, currentChar()); break;
            case '}': token = makeToken(TokenType::RIGHT_BRACE, currentChar()); break;
            case '(': token = makeToken(TokenType::LEFT_PAREN, currentChar()); break;
            case ')': token = makeToken(TokenType::RIGHT_PAREN, currentChar()); break;
            case '[': token = makeToken(TokenType::LEFT_BRACKET, currentChar()); break;
            case ']': token = makeToken(TokenType::RIGHT_BRACKET, currentChar()); break;
            case ':': token = makeToken(TokenType::COLON, currentChar()); break;
            case '=': token = makeToken(TokenType::EQUALS, currentChar()); break;
            case ';': token = makeToken(TokenType::SEMICOLON, currentChar()); break;
            case ',': token = makeToken(TokenType::COMMA, currentChar()); break;
            case '.': token = makeToken(TokenType::DOT, currentChar()); break;
            case '@': token = makeToken(TokenType::AT, currentChar()); break;
            case '+': token = makeToken(TokenType::PLUS, currentChar()); break;
            case '-': token = makeToken(TokenType::MINUS, currentChar()); break;
            case '*': token = makeToken(TokenType::STAR, currentChar()); break;
            case '/': token = makeToken(TokenType::SLASH, currentChar()); break;
            default: token = makeToken(TokenType::UNKNOWN, currentChar()); break;
        }
        advance();
        return token;
    }

    return {TokenType::END_OF_FILE, "", m_line, m_column};
}

std::vector<Token> CHTLLexer::getAllTokens() {
    std::vector<Token> tokens;
    Token token;
    do {
        token = getNextToken();
        tokens.push_back(token);
    } while (token.type != TokenType::END_OF_FILE);
    return tokens;
}

}
