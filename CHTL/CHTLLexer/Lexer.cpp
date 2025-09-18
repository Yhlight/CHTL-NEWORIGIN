#include "Lexer.h"
#include <map>

namespace CHTL {

static std::map<std::string, TokenType> keywords = {
    {"text", TokenType::KEYWORD_TEXT},
    {"style", TokenType::KEYWORD_STYLE},
    {"script", TokenType::KEYWORD_SCRIPT}
};

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        start = current;
        char c = advance();
        switch (c) {
            case '{': tokens.push_back(makeToken(TokenType::LEFT_BRACE)); break;
            case '}': tokens.push_back(makeToken(TokenType::RIGHT_BRACE)); break;
            case ':': tokens.push_back(makeToken(TokenType::COLON)); break;
            case '=': tokens.push_back(makeToken(TokenType::EQUALS)); break;
            case ';': tokens.push_back(makeToken(TokenType::SEMICOLON)); break;

            case '/':
                if (match('/')) {
                    while (peek() != '\n' && !isAtEnd()) advance();
                    tokens.push_back(makeToken(TokenType::SINGLE_LINE_COMMENT, source.substr(start, current - start)));
                } else if (match('*')) {
                    while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                        if (peek() == '\n') {
                            line++;
                            column = 1;
                        }
                        advance();
                    }
                    if (!isAtEnd()) {
                       advance(); // consume *
                       advance(); // consume /
                    }
                    tokens.push_back(makeToken(TokenType::MULTI_LINE_COMMENT, source.substr(start, current - start)));
                }
                break;

            case '#':
                while (peek() != '\n' && !isAtEnd()) advance();
                tokens.push_back(makeToken(TokenType::GENERATOR_COMMENT, source.substr(start, current - start)));
                break;

            case ' ':
            case '\r':
            case '\t':
                // Ignore whitespace.
                break;

            case '\n':
                line++;
                column = 1;
                break;

            case '"':
            case '\'':
                tokens.push_back(stringLiteral());
                break;

            default:
                if (isalpha(c) || c == '_') {
                    tokens.push_back(identifier());
                } else {
                    tokens.push_back(makeToken(TokenType::UNKNOWN, std::string(1, c)));
                }
                break;
        }
    }
    tokens.push_back({TokenType::END_OF_FILE, "", line, column});
    return tokens;
}

bool Lexer::isAtEnd() {
    return current >= source.length();
}

char Lexer::advance() {
    column++;
    return source[current++];
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    column++;
    return true;
}

Token Lexer::makeToken(TokenType type) {
    return {type, source.substr(start, current - start), line, column - (int)(current - start)};
}

Token Lexer::makeToken(TokenType type, const std::string& value) {
    return {type, value, line, column - (int)value.length()};
}

Token Lexer::stringLiteral() {
    char quoteType = source[start];
    while (peek() != quoteType && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        advance();
    }

    if (isAtEnd()) {
        return makeToken(TokenType::UNKNOWN, "Unterminated string.");
    }

    advance(); // The closing quote.
    std::string value = source.substr(start + 1, current - start - 2);
    return makeToken(TokenType::STRING_LITERAL, value);
}

Token Lexer::identifier() {
    while (isalnum(peek()) || peek() == '_') {
        advance();
    }

    std::string text = source.substr(start, current - start);
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        return makeToken(it->second);
    }

    // Per CHTL.md, unquoted literals are a thing.
    // We will treat any identifier-like sequence that is not a keyword as an UNQUOTED_LITERAL
    // The parser will later decide if it's a valid element name, attribute, or just a value.
    return makeToken(TokenType::UNQUOTED_LITERAL);
}

} // namespace CHTL
