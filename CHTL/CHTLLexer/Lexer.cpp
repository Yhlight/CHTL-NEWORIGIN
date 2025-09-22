#include "Lexer.h"
#include <iostream>
#include <cctype>

CHTLLexer::CHTLLexer(std::string source) : source(std::move(source)) {}

std::vector<Token> CHTLLexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }

    tokens.emplace_back(TokenType::END_OF_FILE, "", line, column);
    return tokens;
}

bool CHTLLexer::isAtEnd() {
    return current >= source.length();
}

char CHTLLexer::advance() {
    column++;
    return source[current++];
}

char CHTLLexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char CHTLLexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool CHTLLexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    column++;
    return true;
}

void CHTLLexer::scanToken() {
    char c = advance();
    switch (c) {
        case '{': tokens.emplace_back(TokenType::LEFT_BRACE, "{", line, column); break;
        case '}': tokens.emplace_back(TokenType::RIGHT_BRACE, "}", line, column); break;
        case '[': tokens.emplace_back(TokenType::LEFT_BRACKET, "[", line, column); break;
        case ']': tokens.emplace_back(TokenType::RIGHT_BRACKET, "]", line, column); break;
        case '(': tokens.emplace_back(TokenType::LEFT_PAREN, "(", line, column); break;
        case ')': tokens.emplace_back(TokenType::RIGHT_PAREN, ")", line, column); break;
        case '@': tokens.emplace_back(TokenType::AT, "@", line, column); break;
        case ';': tokens.emplace_back(TokenType::SEMICOLON, ";", line, column); break;
        case ':': tokens.emplace_back(TokenType::COLON, ":", line, column); break;
        case '=': tokens.emplace_back(TokenType::EQUALS, "=", line, column); break;
        case '.': tokens.emplace_back(TokenType::DOT, ".", line, column); break;
        case '&': tokens.emplace_back(TokenType::AMPERSAND, "&", line, column); break;

        case '+': tokens.emplace_back(TokenType::PLUS, "+", line, column); break;
        case '-': tokens.emplace_back(TokenType::MINUS, "-", line, column); break;
        case '%': tokens.emplace_back(TokenType::PERCENT, "%", line, column); break;
        case '*':
            if (match('*')) {
                tokens.emplace_back(TokenType::STAR_STAR, "**", line, column);
            } else {
                tokens.emplace_back(TokenType::STAR, "*", line, column);
            }
            break;

        case '/':
            if (match('/')) {
                singleLineComment();
            } else if (match('*')) {
                multiLineComment();
            } else {
                tokens.emplace_back(TokenType::SLASH, "/", line, column);
            }
            break;
        case '#':
            if (peek() == ' ') {
                generatorComment();
            } else {
                tokens.emplace_back(TokenType::HASH, "#", line, column);
            }
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

        case '"': stringLiteral('"'); break;
        case '\'': stringLiteral('\''); break;

        default:
            if (std::isdigit(c)) {
                number();
            }
            else if (std::isalpha(c) || c == '_') {
                identifier();
            }
            else {
                // For now, treat unknown characters as part of an unquoted literal
                if (!isspace(c)) {
                    unquotedLiteral();
                }
            }
            break;
    }
}

void CHTLLexer::singleLineComment() {
    while (peek() != '\n' && !isAtEnd()) {
        advance();
    }
    tokens.emplace_back(TokenType::SINGLE_LINE_COMMENT, source.substr(start, current - start), line, column);
}

void CHTLLexer::multiLineComment() {
    while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        advance();
    }

    if (isAtEnd()) {
        // Unterminated comment, handle error later
        return;
    }

    // Consume the closing */
    advance();
    advance();
    tokens.emplace_back(TokenType::MULTI_LINE_COMMENT, source.substr(start, current - start), line, column);
}

void CHTLLexer::generatorComment() {
    // consume the space
    advance();
    start = current;
    while (peek() != '\n' && !isAtEnd()) {
        advance();
    }
    tokens.emplace_back(TokenType::GENERATOR_COMMENT, source.substr(start, current - start), line, column);
}


void CHTLLexer::stringLiteral(char quote_type) {
    while (peek() != quote_type && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd()) {
        // Unterminated string, handle error later
        return;
    }

    // The closing quote.
    advance();

    // Trim the surrounding quotes.
    std::string value = source.substr(start + 1, current - start - 2);
    TokenType type = (quote_type == '"') ? TokenType::STRING_LITERAL : TokenType::SINGLE_QUOTED_LITERAL;
    tokens.emplace_back(type, value, line, column);
}

void CHTLLexer::identifier() {
    while (std::isalnum(peek()) || peek() == '_' || peek() == '-') advance();
    std::string text = source.substr(start, current - start);
    TokenType type = TokenType::IDENTIFIER;
    if (text == "inherit") type = TokenType::KEYWORD_INHERIT;
    tokens.emplace_back(type, text, line, column);
}

void CHTLLexer::number() {
    while (std::isdigit(peek())) advance();

    // Look for a fractional part.
    if (peek() == '.' && std::isdigit(peekNext())) {
        // Consume the "."
        advance();
        while (std::isdigit(peek())) advance();
    }

    tokens.emplace_back(TokenType::NUMBER, source.substr(start, current - start), line, column);
}

// This is a simplification for now. CHTL's unquoted literals are more complex,
// especially inside style blocks. This will be refined later.
void CHTLLexer::unquotedLiteral() {
     // An unquoted literal ends with a semicolon, a brace, or a newline.
    while (peek() != ';' && peek() != '{' && peek() != '}' && peek() != '\n' && !isAtEnd()) {
        advance();
    }
    // We backup one character because the advance() in scanToken() already consumed the first char.
    // So we need to backup one char to get the correct start position.
    std::string text = source.substr(start, current - start);

    // remove leading and trailing whitespace
    size_t first = text.find_first_not_of(" \t\r\n");
    if (std::string::npos == first)
    {
        return; // all whitespace
    }
    size_t last = text.find_last_not_of(" \t\r\n");
    tokens.emplace_back(TokenType::UNQUOTED_LITERAL, text.substr(first, (last - first + 1)), line, column);
}
