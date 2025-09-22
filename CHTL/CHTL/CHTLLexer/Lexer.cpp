#include "Lexer.h"
#include <iostream>

namespace CHTL {

const std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"text", TokenType::KEYWORD_TEXT},
    {"style", TokenType::KEYWORD_STYLE},
    {"script", TokenType::KEYWORD_SCRIPT},
    {"[Template]", TokenType::KEYWORD_TEMPLATE},
    {"[Custom]", TokenType::KEYWORD_CUSTOM},
    {"[Origin]", TokenType::KEYWORD_ORIGIN},
    {"[Import]", TokenType::KEYWORD_IMPORT},
    {"[Namespace]", TokenType::KEYWORD_NAMESPACE},
    {"[Configuration]", TokenType::KEYWORD_CONFIG},
    {"use", TokenType::KEYWORD_USE},
    {"delete", TokenType::KEYWORD_DELETE},
    {"insert", TokenType::KEYWORD_INSERT},
    {"from", TokenType::KEYWORD_FROM},
    {"as", TokenType::KEYWORD_AS},
    {"except", TokenType::KEYWORD_EXCEPT},
    {"inherit", TokenType::KEYWORD_INHERIT},
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
    if (!isAtEnd()) {
        if (source[current] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        current++;
        return source[current - 1];
    }
    return '\0';
}

bool Lexer::match(char expected) {
    if (isAtEnd() || source[current] != expected) {
        return false;
    }
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

Token Lexer::makeToken(TokenType type) {
    return {type, source.substr(start, current - start), line, column - (int)(current - start)};
}

Token Lexer::errorToken(const std::string& message) {
    return {TokenType::ILLEGAL, message, line, column};
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '(': tokens.push_back(makeToken(TokenType::LEFT_PAREN)); break;
        case ')': tokens.push_back(makeToken(TokenType::RIGHT_PAREN)); break;
        case '{': tokens.push_back(makeToken(TokenType::LEFT_BRACE)); break;
        case '}': tokens.push_back(makeToken(TokenType::RIGHT_BRACE)); break;
        case '[': tokens.push_back(makeToken(TokenType::LEFT_BRACKET)); break;
        case ']': tokens.push_back(makeToken(TokenType::RIGHT_BRACKET)); break;
        case ',': tokens.push_back(makeToken(TokenType::COMMA)); break;
        case '.': tokens.push_back(makeToken(TokenType::DOT)); break;
        case '-': tokens.push_back(makeToken(TokenType::MINUS)); break;
        case '+': tokens.push_back(makeToken(TokenType::PLUS)); break;
        case ';': tokens.push_back(makeToken(TokenType::SEMICOLON)); break;
        case '%': tokens.push_back(makeToken(TokenType::PERCENT)); break;
        case '?': tokens.push_back(makeToken(TokenType::QUESTION)); break;
        case '@': tokens.push_back(makeToken(TokenType::AT_SIGN)); break;
        case '&': tokens.push_back(makeToken(TokenType::AMPERSAND)); break;
        case ':': tokens.push_back(makeToken(TokenType::COLON)); break;
        case '=': tokens.push_back(makeToken(TokenType::EQUALS)); break;
        case '<': tokens.push_back(makeToken(TokenType::LESS)); break;
        case '>': tokens.push_back(makeToken(TokenType::GREATER)); break;

        case '*':
            tokens.push_back(makeToken(TokenType::ASTERISK));
            break;

        case '#':
            // Generator comments are treated as significant tokens
            while (peek() != '\n' && !isAtEnd()) {
                advance();
            }
            tokens.push_back(makeToken(TokenType::COMMENT));
            break;

        case '/':
            if (match('/')) { // A // comment goes until the end of the line.
                while (peek() != '\n' && !isAtEnd()) advance();
            } else if (match('*')) { // A /* */ comment.
                while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                    advance();
                }
                if (!isAtEnd()) {
                    advance(); // consume '*'
                    advance(); // consume '/'
                }
            } else {
                tokens.push_back(makeToken(TokenType::SLASH));
            }
            break;

        // Ignore whitespace.
        case ' ':
        case '\r':
        case '\t':
            break;

        case '\n':
            // Handled in advance()
            break;

        case '"':
        case '\'':
            stringLiteral(c);
            break;

        default:
            if (isalpha(c) || c == '_') {
                identifier();
            } else if (isdigit(c)) {
                // For now, CHTL treats numbers as part of unquoted literals
                // This might need refinement later based on parsing context.
                unquotedLiteral();
            } else {
                // The spec mentions unquoted literals extensively.
                // This will be a common case.
                unquotedLiteral();
            }
            break;
    }
}

void Lexer::stringLiteral(char quoteType) {
    while (peek() != quoteType && !isAtEnd()) {
        advance();
    }

    if (isAtEnd()) {
        tokens.push_back(errorToken("Unterminated string."));
        return;
    }

    advance(); // The closing quote.

    // Trim the surrounding quotes.
    std::string value = source.substr(start + 1, current - start - 2);
    tokens.push_back({TokenType::STRING_LITERAL, value, line, column - (int)value.length()});
}

void Lexer::identifier() {
    while (isalnum(peek()) || peek() == '_') {
        advance();
    }

    std::string text = source.substr(start, current - start);
    auto it = keywords.find(text);
    TokenType type = (it != keywords.end()) ? it->second : TokenType::IDENTIFIER;
    tokens.push_back(makeToken(type));
}

// CHTL allows for unquoted literals that can contain a mix of characters.
// This is a simplified handler. The parser will be responsible for validation.
void Lexer::unquotedLiteral() {
    // Greedily consume until a character that cannot be part of an unquoted literal.
    // These are typically delimiters or operators.
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n' ||
            c == '{' || c == '}' || c == ';' || c == ':' || c == ',') {
            break;
        }
        advance();
    }

    std::string text = source.substr(start, current - start);

    // Trim trailing whitespace from the literal if any was included
    size_t end = text.find_last_not_of(" \t\r\n");
    if (std::string::npos != end) {
        text = text.substr(0, end + 1);
    }

    // Check if the extracted text is a keyword
    auto it = keywords.find(text);
    TokenType type = (it != keywords.end()) ? it->second : TokenType::UNQUOTED_LITERAL;

    tokens.push_back({type, text, line, column - (int)text.length()});
}


} // namespace CHTL
