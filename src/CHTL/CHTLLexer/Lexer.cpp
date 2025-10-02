#include "Lexer.h"
#include <iostream>
#include <unordered_map>

std::unordered_map<std::string, TokenType> keywords = {
    {"text", TokenType::TEXT},
    {"style", TokenType::STYLE},
    {"script", TokenType::SCRIPT},
    {"[Template]", TokenType::TEMPLATE},
    {"[Custom]", TokenType::CUSTOM},
    {"[Origin]", TokenType::ORIGIN},
    {"[Import]", TokenType::IMPORT},
    {"[Namespace]", TokenType::NAMESPACE},
    {"use", TokenType::USE},
    {"except", TokenType::EXCEPT},
    {"from", TokenType::FROM},
    {"as", TokenType::AS},
    {"delete", TokenType::DELETE},
    {"insert", TokenType::INSERT},
    {"after", TokenType::AFTER},
    {"before", TokenType::BEFORE},
    {"replace", TokenType::REPLACE},
    {"at", TokenType::AT_TOP}, // Simplified for now
    {"inherit", TokenType::INHERIT},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE}
};

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }

    tokens.push_back({TokenType::END_OF_FILE, "", line, column});
    return tokens;
}

bool Lexer::isAtEnd() {
    return current >= source.length();
}

char Lexer::advance() {
    current++;
    column++;
    return source[current - 1];
}

void Lexer::addToken(TokenType type) {
    addToken(type, "");
}

void Lexer::addToken(TokenType type, const std::string& literal) {
    std::string text = source.substr(start, current - start);
    if (type == TokenType::STRING) {
        tokens.push_back({type, literal, line, column});
    } else {
        tokens.push_back({type, text, line, column});
    }
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    column++;
    return true;
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case '[': addToken(TokenType::LEFT_BRACKET); break;
        case ']': addToken(TokenType::RIGHT_BRACKET); break;
        case ',': addToken(TokenType::COMMA); break;
        case '.': addToken(TokenType::DOT); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case ':': addToken(TokenType::COLON); break;
        case '+': addToken(TokenType::PLUS); break;
        case '-': addToken(TokenType::MINUS); break;
        case '*':
            if (match('*')) {
                addToken(TokenType::STAR_STAR);
            } else {
                addToken(TokenType::STAR);
            }
            break;
        case '/':
            if (match('/')) {
                singleLineComment();
            } else if (match('*')) {
                blockComment();
            } else {
                addToken(TokenType::SLASH);
            }
            break;
        case '%': addToken(TokenType::PERCENT); break;
        case '=': addToken(TokenType::EQUAL); break;
        case '>': addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;
        case '<': addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
        case '&':
            if (match('&')) {
                addToken(TokenType::AMPERSAND_AMPERSAND);
            }
            break;
        case '|':
            if (match('|')) {
                addToken(TokenType::PIPE_PIPE);
            }
            break;
        case '#':
            generatorComment();
            break;

        // Ignore whitespace
        case ' ':
        case '\r':
        case '\t':
            break;

        case '\n':
            line++;
            column = 1;
            break;

        case '"':
        case '\'':
            stringLiteral();
            break;

        default:
            if (isdigit(c)) {
                numberLiteral();
            } else if (isalpha(c) || c == '_') {
                identifier();
            } else {
                std::cerr << "Unexpected character at line " << line << ", column " << column << ": " << c << std::endl;
            }
            break;
    }
}

void Lexer::stringLiteral() {
    char quote_type = source[start];
    while (peek() != quote_type && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        advance();
    }

    if (isAtEnd()) {
        std::cerr << "Unterminated string at line " << line << ", column " << column << std::endl;
        return;
    }

    // The closing quote.
    advance();

    // Trim the surrounding quotes.
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING, value);
}

void Lexer::numberLiteral() {
    while (isdigit(peek())) advance();

    if (peek() == '.' && isdigit(peekNext())) {
        advance();
        while (isdigit(peek())) advance();
    }

    addToken(TokenType::NUMBER, source.substr(start, current - start));
}

void Lexer::identifier() {
    while (isalnum(peek()) || peek() == '_') advance();

    std::string text = source.substr(start, current - start);
    TokenType type = TokenType::IDENTIFIER;
    if (keywords.count(text)) {
        type = keywords[text];
    } else if (text == "[Template]" || text == "[Custom]" || text == "[Origin]" || text == "[Import]" || text == "[Namespace]") {
        type = keywords[text];
    }

    addToken(type);
}

void Lexer::singleLineComment() {
    while (peek() != '\n' && !isAtEnd()) {
        advance();
    }
}

void Lexer::blockComment() {
    while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        advance();
    }

    if (isAtEnd()) {
        std::cerr << "Unterminated block comment at line " << line << std::endl;
        return;
    }

    // Consume the "*/"
    advance();
    advance();
}

void Lexer::generatorComment() {
    while (peek() != '\n' && !isAtEnd()) {
        advance();
    }
}