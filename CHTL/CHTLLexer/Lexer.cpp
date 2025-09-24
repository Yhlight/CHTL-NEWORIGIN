#include "Lexer.h"
#include <cctype>

namespace CHTL {

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::Tokenize() {
    std::vector<Token> tokens;
    Token token;
    do {
        token = GetNextToken();
        tokens.push_back(token);
    } while (token.type != TokenType::END_OF_FILE);
    return tokens;
}

void Lexer::SkipSingleLineComment() {
    while (Peek() != '\n' && current < source.length()) {
        Advance();
    }
}

void Lexer::SkipMultiLineComment() {
    Advance();
    while (current < source.length()) {
        if (Peek() == '*' && (current + 1 < source.length() && source[current + 1] == '/')) {
            Advance(); Advance();
            return;
        }
        Advance();
    }
}

Token Lexer::GetNextToken() {
    while (true) {
        SkipWhitespace();

        if (current >= source.length()) return {TokenType::END_OF_FILE, ""};

        if (Peek() == '/') {
            if (current + 1 < source.length()) {
                if (source[current + 1] == '/') {
                    Advance(); Advance(); SkipSingleLineComment(); continue;
                }
                if (source[current + 1] == '*') {
                    Advance(); Advance(); SkipMultiLineComment(); continue;
                }
            }
        }

        if (isdigit(Peek())) {
            size_t start = current;
            while (isdigit(Peek()) || Peek() == '.') Advance(); // Allow decimals
            return {TokenType::NUMBER, source.substr(start, current - start)};
        }

        char c = Advance();

        if (isalpha(c) || c == '_') {
            current--;
            return MakeIdentifier();
        }

        switch (c) {
            case '#': // Can be a comment or a hex color hash
                if (Peek() == ' ') { // It's a generator comment
                    Advance();
                    size_t start = current;
                    while (Peek() != '\n' && current < source.length()) Advance();
                    return {TokenType::COMMENT, source.substr(start, current - start)};
                } else { // It's a HASH for a color
                    return {TokenType::HASH, "#"};
                }
            case '{': return {TokenType::LEFT_BRACE, "{"};
            case '}': return {TokenType::RIGHT_BRACE, "}"};
            case '[': return {TokenType::LEFT_BRACKET, "["};
            case ']': return {TokenType::RIGHT_BRACKET, "]"};
            case '(': return {TokenType::LEFT_PAREN, "("};
            case ')': return {TokenType::RIGHT_PAREN, ")"};
            case '@': return {TokenType::AT, "@"};
            case ':': return {TokenType::COLON, ":"};
            case ';': return {TokenType::SEMICOLON, ";"};
            case '"': return MakeString();
            case '+': return {TokenType::PLUS, "+"};
            case '-': return {TokenType::MINUS, "-"};
            case '*': return {TokenType::STAR, "*"};
            case '/': return {TokenType::SLASH, "/"};
            case '>': return {TokenType::GREATER, ">"};
            case '?': return {TokenType::QUESTION, "?"};
        }

        return {TokenType::UNKNOWN, std::string(1, c)};
    }
}

void Lexer::SkipWhitespace() {
    while (current < source.length() && isspace(Peek())) Advance();
}

char Lexer::Peek() {
    if (current >= source.length()) return '\0';
    return source[current];
}

char Lexer::Advance() {
    if (current < source.length()) return source[current++];
    return '\0';
}

Token Lexer::MakeIdentifier() {
    size_t start = current;
    while (isalnum(Peek()) || Peek() == '_' || Peek() == '-') Advance();
    return {TokenType::IDENTIFIER, source.substr(start, current - start)};
}

Token Lexer::MakeString() {
    size_t start = current;
    while (Peek() != '"' && current < source.length()) Advance();
    if (current >= source.length()) return {TokenType::UNKNOWN, source.substr(start - 1, current - (start - 1))};
    Advance();
    return {TokenType::STRING, source.substr(start, current - start - 1)};
}

std::string TokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::AT: return "AT";
        case TokenType::HASH: return "HASH";
        case TokenType::COLON: return "COLON";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING: return "STRING";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::COMMENT: return "COMMENT";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::STAR: return "STAR";
        case TokenType::SLASH: return "SLASH";
        case TokenType::GREATER: return "GREATER";
        case TokenType::QUESTION: return "QUESTION";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::UNKNOWN: return "UNKNOWN";
        default: return "INVALID_TOKEN";
    }
}

} // namespace CHTL
