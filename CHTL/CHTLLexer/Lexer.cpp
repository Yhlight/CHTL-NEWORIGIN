#include "Lexer.h"
#include <iostream>

namespace CHTL {

// Initialize the keyword map
const std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"inherit", TokenType::KEYWORD_INHERIT},
    {"delete", TokenType::KEYWORD_DELETE},
    {"insert", TokenType::KEYWORD_INSERT},
    {"after", TokenType::KEYWORD_AFTER},
    {"before", TokenType::KEYWORD_BEFORE},
    {"replace", TokenType::KEYWORD_REPLACE},
    {"from", TokenType::KEYWORD_FROM},
    {"as", TokenType::KEYWORD_AS},
    {"except", TokenType::KEYWORD_EXCEPT},
    {"use", TokenType::KEYWORD_USE},
    {"text", TokenType::KEYWORD_TEXT},
    {"style", TokenType::KEYWORD_STYLE},
    {"script", TokenType::KEYWORD_SCRIPT},
    {"if", TokenType::KEYWORD_IF},
    {"else", TokenType::KEYWORD_ELSE},
    // "at top" and "at bottom" will be handled as multiple tokens
};

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::tokenizeAll() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }
    tokens.push_back({TokenType::END_OF_FILE, "", line, column});
    return tokens;
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case '[': bracketedKeyword(); break; // Simplified for now
        case ']': addToken(TokenType::RIGHT_BRACKET); break;
        case ',': addToken(TokenType::COMMA); break;
        case '.': addToken(TokenType::DOT); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case ':': addToken(TokenType::COLON); break;
        case '?': addToken(TokenType::QUESTION); break;
        case '@': addToken(TokenType::AT_KEYWORD); break; // Simplified
        case '#': genComment(); break;

        case '-': addToken(match('>') ? TokenType::ARROW : TokenType::MINUS); break;
        case '+': addToken(TokenType::PLUS); break;
        case '*': addToken(match('*') ? TokenType::POWER : TokenType::STAR); break;
        case '/':
            if (match('/')) { lineComment(); }
            else if (match('*')) { blockComment(); }
            else { addToken(TokenType::SLASH); }
            break;

        case '&':
            if (match('-')) { if (match('>')) addToken(TokenType::EVENT_ARROW); }
            else if (match('&')) { addToken(TokenType::AND); }
            break;
        case '|': if (match('|')) { addToken(TokenType::OR); } break;
        case '=': addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
        case '!': if (match('=')) addToken(TokenType::NOT_EQUAL); break;
        case '<': addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
        case '>': addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;

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
            if (isDigit(c)) {
                numberLiteral();
            } else if (isAlpha(c)) {
                identifier();
            } else {
                addToken(TokenType::UNKNOWN, std::string(1, c));
            }
            break;
    }
}

bool Lexer::isAtEnd() {
    return current >= source.length();
}

char Lexer::advance() {
    column++;
    return source[current++];
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

void Lexer::addToken(TokenType type) {
    addToken(type, "");
}

void Lexer::addToken(TokenType type, const std::string& literal) {
    std::string text = source.substr(start, current - start);
    tokens.push_back({type, text, line, column - (int)text.length()});
}

void Lexer::stringLiteral() {
    char quoteType = source[current - 1];
    while (peek() != quoteType && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            column = 0; // Will be incremented by advance()
        }
        advance();
    }

    if (isAtEnd()) {
        addToken(TokenType::UNKNOWN, "Unterminated string.");
        return;
    }

    advance(); // Closing quote
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING_LITERAL, value);
}

void Lexer::numberLiteral() {
    while (isDigit(peek())) advance();
    if (peek() == '.' && isDigit(peekNext())) {
        advance();
        while (isDigit(peek())) advance();
    }
    addToken(TokenType::NUMBER_LITERAL, source.substr(start, current - start));
}

void Lexer::identifier() {
    while (isAlphaNumeric(peek())) advance();

    std::string text = source.substr(start, current - start);
    TokenType type = TokenType::IDENTIFIER;
    if (keywords.count(text)) {
        type = keywords.at(text);
    }
    // A special case for unquoted literals which are common in CHTL
    else if (peek() == ' ' || peek() == '\n' || peek() == ';' || peek() == '}') {
        // This is a simplification. The parser will have to decide if an
        // identifier is truly an unquoted literal based on context.
        // For now, we can have a more generic approach or just use IDENTIFIER.
        // Let's stick with IDENTIFIER and let the parser handle context.
    }

    addToken(type);
}

void Lexer::lineComment() {
    while (peek() != '\n' && !isAtEnd()) {
        advance();
    }
}

void Lexer::blockComment() {
    while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            column = 0;
        }
        advance();
    }
    if (!isAtEnd()) {
        advance(); // *
        advance(); // /
    }
}

void Lexer::genComment() {
    // This is a generator comment, treat like a line comment for now
    lineComment();
}

void Lexer::bracketedKeyword() {
    // Simplified: treat everything between [ and ] as a single token.
    while (peek() != ']' && !isAtEnd()) {
        advance();
    }
    if (peek() == ']') {
        // advance(); // Consume ']' but don't include it in the lexeme
        addToken(TokenType::BRACKETED_KEYWORD, source.substr(start, current - start));
    } else {
        addToken(TokenType::UNKNOWN, "Unterminated bracketed keyword.");
    }
}

bool Lexer::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool Lexer::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
            c == '_';
}

bool Lexer::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

} // namespace CHTL
