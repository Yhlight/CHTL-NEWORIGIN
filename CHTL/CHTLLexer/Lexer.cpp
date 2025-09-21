#include "Lexer.h"
#include <iostream>
#include <map>

namespace CHTL {

const std::map<std::string, TokenType> Lexer::keywords = {
    {"text", TokenType::KEYWORD_TEXT},
    {"style", TokenType::KEYWORD_STYLE},
    {"script", TokenType::KEYWORD_SCRIPT},
    {"inherit", TokenType::KEYWORD_INHERIT},
    {"delete", TokenType::KEYWORD_DELETE},
    {"insert", TokenType::KEYWORD_INSERT},
    {"after", TokenType::KEYWORD_AFTER},
    {"before", TokenType::KEYWORD_BEFORE},
    {"replace", TokenType::KEYWORD_REPLACE},
    {"at", TokenType::KEYWORD_AT},
    {"top", TokenType::KEYWORD_TOP},
    {"bottom", TokenType::KEYWORD_BOTTOM},
    {"from", TokenType::KEYWORD_FROM},
    {"as", TokenType::KEYWORD_AS},
    {"except", TokenType::KEYWORD_EXCEPT},
    {"use", TokenType::KEYWORD_USE},
    {"html5", TokenType::KEYWORD_HTML5},
    {"[Template]", TokenType::KEYWORD_TEMPLATE},
    {"[Custom]", TokenType::KEYWORD_CUSTOM},
    {"[Origin]", TokenType::KEYWORD_ORIGIN},
    {"[Import]", TokenType::KEYWORD_IMPORT},
    {"[Namespace]", TokenType::KEYWORD_NAMESPACE},
    {"[Configuration]", TokenType::KEYWORD_CONFIGURATION},
    {"@Style", TokenType::AT_STYLE},
    {"@Element", TokenType::AT_ELEMENT},
    {"@Var", TokenType::AT_VAR},
    {"@Html", TokenType::AT_HTML},
    {"@Style", TokenType::AT_STYLE},
    {"@JavaScript", TokenType::AT_JAVASCRIPT},
    {"@Chtl", TokenType::AT_CHTL},
    {"@CJmod", TokenType::AT_CJMOD},
    {"@Config", TokenType::AT_CONFIG},
};


Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }

    tokens.push_back({TokenType::END_OF_FILE, "", line, col_start});
    return tokens;
}

bool Lexer::isAtEnd() {
    return current >= source.length();
}

char Lexer::advance() {
    current++;
    return source[current - 1];
}

void Lexer::addToken(TokenType type) {
    std::string text = source.substr(start, current - start);
    tokens.push_back({type, text, line, col_start});
}

void Lexer::addToken(TokenType type, const std::string& literal) {
    tokens.push_back({type, literal, line, col_start});
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
    return true;
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case '[':
            // This is a simple lookahead for special keywords. A more robust
            // solution would be a state machine or a more advanced lexer.
            if (source.substr(current, 7) == "Import]") {
                current += 7;
                addToken(TokenType::KEYWORD_IMPORT);
            } else if (source.substr(current, 9) == "Template]") {
                current += 9;
                addToken(TokenType::KEYWORD_TEMPLATE);
            } else if (source.substr(current, 7) == "Custom]") {
                current += 7;
                addToken(TokenType::KEYWORD_CUSTOM);
            } else if (source.substr(current, 7) == "Origin]") {
                current += 7;
                addToken(TokenType::KEYWORD_ORIGIN);
            } else if (source.substr(current, 10) == "Namespace]") {
                current += 10;
                addToken(TokenType::KEYWORD_NAMESPACE);
            } else if (source.substr(current, 14) == "Configuration]") {
                current += 14;
                addToken(TokenType::KEYWORD_CONFIGURATION);
            }
            else {
                addToken(TokenType::LEFT_BRACKET);
            }
            break;
        case ']': addToken(TokenType::RIGHT_BRACKET); break;
        case ',': addToken(TokenType::COMMA); break;
        case '.': addToken(TokenType::DOT); break;
        case '-': addToken(TokenType::MINUS); break;
        case '+': addToken(TokenType::PLUS); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '%': addToken(TokenType::PERCENT); break;
        case '*':
            addToken(match('*') ? TokenType::STAR_STAR : TokenType::STAR);
            break;
        case ':': addToken(TokenType::COLON); break;
        case '=':
             addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
             break;
        case '<':
            addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
            break;
        case '>':
            addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
            break;
        case '!':
             addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::IDENTIFIER); // Or some error token
             break;
        case '/':
            if (match('/')) {
                // A comment goes until the end of the line.
                while (peek() != '\n' && !isAtEnd()) advance();
            } else if (match('*')) {
                scanBlockComment();
            }
            else {
                addToken(TokenType::SLASH);
            }
            break;

        case '#':
            if (peek() == ' ') {
                 // generator comment
                while (peek() != '\n' && !isAtEnd()) advance();
            } else {
                addToken(TokenType::HASH);
            }
            break;

        case '&':
            if (match('&')) {
                addToken(TokenType::AMPERSAND_AMPERSAND);
            } else {
                addToken(TokenType::AMPERSAND);
            }
            break;

        case '@':
            while (isalpha(peek())) advance();
            addToken(keywords.at(source.substr(start, current-start)));
            break;

        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace.
            break;

        case '\n':
            line++;
            col_start = 1;
            break;

        case '"': scanString('"'); break;
        case '\'': scanString('\''); break;

        default:
            if (isdigit(c)) {
                scanNumber();
            } else if (isalpha(c) || c == '_') {
                scanIdentifier();
            } else {
                // For now, treat other characters as part of an unquoted literal
                scanUnquotedLiteral();
            }
            break;
    }
}

void Lexer::scanString(char quote) {
    while (peek() != quote && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            col_start = 0; // will be 1 after advance
        }
        advance();
    }

    if (isAtEnd()) {
        // Unterminated string.
        // Handle error
        return;
    }

    // The closing quote.
    advance();

    // Trim the surrounding quotes.
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING, value);
}

void Lexer::scanNumber() {
    while (isdigit(peek())) advance();

    // Look for a fractional part.
    if (peek() == '.' && isdigit(peekNext())) {
        // Consume the "."
        advance();

        while (isdigit(peek())) advance();
    }

    addToken(TokenType::NUMBER, source.substr(start, current - start));
}

void Lexer::scanIdentifier() {
    while (isalnum(peek()) || peek() == '_') advance();

    std::string text = source.substr(start, current - start);
    TokenType type;
    if (keywords.count(text)) {
        type = keywords.at(text);
    } else {
        type = TokenType::IDENTIFIER;
    }
    addToken(type);
}

void Lexer::scanBlockComment() {
    while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            col_start = 0;
        }
        advance();
    }

    if (isAtEnd()) {
        // Unterminated block comment.
        // Handle error
        return;
    }

    // Consume the */
    advance();
    advance();
}

void Lexer::scanUnquotedLiteral() {
    // This is tricky. For now, let's assume it goes until a semicolon, brace, or newline.
    while (peek() != ';' && peek() != '{' && peek() != '}' && peek() != '\n' && !isAtEnd()) {
        advance();
    }
    addToken(TokenType::UNQUOTED_LITERAL, source.substr(start, current - start));
}


} // namespace CHTL
