#include "Lexer.h"
#include <iostream>

namespace CHTL {

const std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"text", TokenType::TEXT},
    {"style", TokenType::STYLE},
    {"script", TokenType::SCRIPT},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"insert", TokenType::INSERT},
    {"delete", TokenType::DELETE},
    {"from", TokenType::FROM},
    {"as", TokenType::AS},
    {"use", TokenType::USE},
    {"except", TokenType::EXCEPT},
    {"inherit", TokenType::INHERIT},
    {"after", TokenType::AFTER},
    {"before", TokenType::BEFORE},
    {"replace", TokenType::REPLACE},
    {"at top", TokenType::KEYWORD_AT_TOP},
    {"at bottom", TokenType::KEYWORD_AT_BOTTOM},
    {"html5", TokenType::HTML5},
    {"[Custom]", TokenType::KEYWORD_CUSTOM},
    {"[Template]", TokenType::KEYWORD_TEMPLATE},
    {"[Origin]", TokenType::KEYWORD_ORIGIN},
    {"[Import]", TokenType::KEYWORD_IMPORT},
    {"[Namespace]", TokenType::KEYWORD_NAMESPACE},
    {"[Configuration]", TokenType::KEYWORD_CONFIGURATION},
    {"[Info]", TokenType::KEYWORD_INFO},
    {"[Export]", TokenType::KEYWORD_EXPORT},
};


Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::scanTokens() {
    while (current < source.length()) {
        start = current;
        scanToken();
    }

    tokens.push_back({TokenType::END_OF_FILE, "", {}, line, column});
    return tokens;
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '{':
            if (match('{')) {
                makeToken(TokenType::LEFT_DOUBLE_BRACE);
            } else {
                makeToken(TokenType::LEFT_BRACE);
            }
            break;
        case '}':
            if (match('}')) {
                makeToken(TokenType::RIGHT_DOUBLE_BRACE);
            } else {
                makeToken(TokenType::RIGHT_BRACE);
            }
            break;
        case '(': makeToken(TokenType::LEFT_PAREN); break;
        case ')': makeToken(TokenType::RIGHT_PAREN); break;
        case '[': makeToken(TokenType::LEFT_BRACKET); break;
        case ']': makeToken(TokenType::RIGHT_BRACKET); break;
        case ':': makeToken(TokenType::COLON); break;
        case ';': makeToken(TokenType::SEMICOLON); break;
        case ',': makeToken(TokenType::COMMA); break;
        case '.': makeToken(TokenType::DOT); break;
        case '?': makeToken(TokenType::QUESTION); break;
        case '@': makeToken(TokenType::AT); break;
        case '#': makeToken(TokenType::HASH); break;
        case '$': makeToken(TokenType::DOLLAR); break;
        case '=': makeToken(TokenType::EQUAL); break;
        case '+': makeToken(TokenType::PLUS); break;
        case '-':
            if (match('>')) {
                makeToken(TokenType::ARROW);
            } else {
                makeToken(TokenType::MINUS);
            }
            break;
        case '*':
            if (match('*')) {
                makeToken(TokenType::STAR_STAR);
            } else {
                makeToken(TokenType::STAR);
            }
            break;
        case '/':
            if (match('/')) {
                lineComment();
            } else if (match('*')) {
                blockComment();
            } else {
                makeToken(TokenType::SLASH);
            }
            break;
        case '%': makeToken(TokenType::PERCENT); break;
        case '&':
            if (match('&')) {
                makeToken(TokenType::AMPERSAND_AMPERSAND);
            } else if (match('-')) {
                if (match('>')) {
                    makeToken(TokenType::AMPERSAND_ARROW);
                } else {
                    // Backtrack the '-'
                    current--;
                    makeToken(TokenType::AMPERSAND);
                }
            }
            else {
                makeToken(TokenType::AMPERSAND);
            }
            break;
        case '|':
            if (match('|')) {
                makeToken(TokenType::PIPE_PIPE);
            } else {
                makeToken(TokenType::PIPE);
            }
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
            string();
            break;

        default:
            if (isdigit(c)) {
                number();
            } else if (isalpha(c) || c == '_') {
                identifier();
            } else {
                makeToken(TokenType::UNRECOGNIZED);
            }
            break;
    }
}

char Lexer::advance() {
    column++;
    return source[current++];
}

bool Lexer::match(char expected) {
    if (current >= source.length()) return false;
    if (source[current] != expected) return false;
    current++;
    column++;
    return true;
}

char Lexer::peek() {
    if (current >= source.length()) return '\0';
    return source[current];
}

char Lexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

void Lexer::string() {
    char quote_type = source[current - 1];
    while (peek() != quote_type && current < source.length()) {
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        advance();
    }

    if (current >= source.length()) {
        // Unterminated string.
        tokens.push_back(errorToken("Unterminated string."));
        return;
    }

    // The closing quote.
    advance();

    // Trim the surrounding quotes.
    std::string value = source.substr(start + 1, current - start - 2);
    std::string lexeme = source.substr(start, current - start);
    Token token = {TokenType::STRING, lexeme, value, line, column - (int)lexeme.length()};
    tokens.push_back(token);
}

void Lexer::number() {
    while (isdigit(peek())) advance();

    // Look for a fractional part.
    if (peek() == '.' && isdigit(peekNext())) {
        // Consume the "."
        advance();
        while (isdigit(peek())) advance();
    }

    std::string num_str = source.substr(start, current - start);
    Token token = {TokenType::NUMBER, num_str, std::stod(num_str), line, column - (int)num_str.length()};
    tokens.push_back(token);
}

void Lexer::identifier() {
    while (isalnum(peek()) || peek() == '_') advance();

    std::string text = source.substr(start, current - start);
    TokenType type;
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        type = it->second;
    } else {
        type = TokenType::IDENTIFIER;
    }
    makeToken(type);
}

void Lexer::lineComment() {
    while (peek() != '\n' && current < source.length()) {
        advance();
    }
}

void Lexer::blockComment() {
    int nesting = 1;
    while (nesting > 0 && current < source.length()) {
        if (peek() == '/' && peekNext() == '*') {
            advance();
            advance();
            nesting++;
        } else if (peek() == '*' && peekNext() == '/') {
            advance();
            advance();
            nesting--;
        } else if (peek() == '\n') {
            line++;
            column = 1;
            advance();
        }
        else {
            advance();
        }
    }
    if (nesting > 0) {
        tokens.push_back(errorToken("Unterminated block comment."));
    }
}

Token Lexer::makeToken(TokenType type) {
    std::string lexeme = source.substr(start, current - start);
    Token token = {type, lexeme, {}, line, column - (int)lexeme.length()};
    tokens.push_back(token);
    return token;
}

Token Lexer::makeToken(TokenType type, const std::string& lexeme) {
    Token token = {type, lexeme, {}, line, column - (int)lexeme.length()};
    tokens.push_back(token);
    return token;
}

Token Lexer::errorToken(const std::string& message) {
    return {TokenType::UNRECOGNIZED, message, {}, line, column};
}

} // namespace CHTL