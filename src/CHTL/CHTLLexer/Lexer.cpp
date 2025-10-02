#include "Lexer.h"
#include <cctype>

namespace CHTL {

Lexer::Lexer(const std::string& source)
    : source_(source), current_(0), line_(1), column_(1) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        Token token = scanToken();
        if (token.type != TokenType::ERROR) {
            tokens.push_back(token);
        }
    }
    tokens.push_back(Token(TokenType::EOF_TOKEN, "", line_, column_));
    return tokens;
}

Token Lexer::scanToken() {
    // Skip whitespace
    while (!isAtEnd() && std::isspace(source_[current_])) {
        if (source_[current_] == '\n') {
            line_++;
            column_ = 1;
        } else {
            column_++;
        }
        current_++;
    }
    
    if (isAtEnd()) return Token(TokenType::EOF_TOKEN, "", line_, column_);
    
    char c = advance();
    int startColumn = column_ - 1;
    
    // Single character tokens
    switch (c) {
        case '{': return Token(TokenType::LEFT_BRACE, "{", line_, startColumn);
        case '}': return Token(TokenType::RIGHT_BRACE, "}", line_, startColumn);
        case '[': return Token(TokenType::LEFT_BRACKET, "[", line_, startColumn);
        case ']': return Token(TokenType::RIGHT_BRACKET, "]", line_, startColumn);
        case '(': return Token(TokenType::LEFT_PAREN, "(", line_, startColumn);
        case ')': return Token(TokenType::RIGHT_PAREN, ")", line_, startColumn);
        case ',': return Token(TokenType::COMMA, ",", line_, startColumn);
        case ';': return Token(TokenType::SEMICOLON, ";", line_, startColumn);
        case ':': return Token(TokenType::COLON, ":", line_, startColumn);
        case '.': return Token(TokenType::DOT, ".", line_, startColumn);
        case '+': return Token(TokenType::PLUS, "+", line_, startColumn);
        case '-': return Token(TokenType::MINUS, "-", line_, startColumn);
        case '/': return Token(TokenType::SLASH, "/", line_, startColumn);
        case '%': return Token(TokenType::PERCENT, "%", line_, startColumn);
        case '@': return Token(TokenType::AT_SYMBOL, "@", line_, startColumn);
        case '&': return Token(TokenType::AMPERSAND, "&", line_, startColumn);
        case '?': return Token(TokenType::QUESTION, "?", line_, startColumn);
        case '*': 
            if (match('*')) return Token(TokenType::DOUBLE_STAR, "**", line_, startColumn);
            return Token(TokenType::STAR, "*", line_, startColumn);
        case '=':
            if (match('=')) return Token(TokenType::EQUAL_EQUAL, "==", line_, startColumn);
            return Token(TokenType::EQUAL, "=", line_, startColumn);
        case '<':
            if (match('=')) return Token(TokenType::LESS_EQUAL, "<=", line_, startColumn);
            return Token(TokenType::LESS, "<", line_, startColumn);
        case '>':
            if (match('=')) return Token(TokenType::GREATER_EQUAL, ">=", line_, startColumn);
            return Token(TokenType::GREATER, ">", line_, startColumn);
        case '"': return string('"');
        case '\'': return string('\'');
    }
    
    // Numbers
    if (std::isdigit(c)) {
        current_--;
        column_--;
        return number();
    }
    
    // Identifiers and keywords
    if (std::isalpha(c) || c == '_') {
        current_--;
        column_--;
        return identifier();
    }
    
    return Token(TokenType::ERROR, std::string(1, c), line_, startColumn);
}

char Lexer::advance() {
    column_++;
    return source_[current_++];
}

char Lexer::peek() const {
    return isAtEnd() ? '\0' : source_[current_];
}

char Lexer::peekNext() const {
    return (current_ + 1 >= source_.length()) ? '\0' : source_[current_ + 1];
}

bool Lexer::isAtEnd() const {
    return current_ >= source_.length();
}

bool Lexer::match(char expected) {
    if (isAtEnd() || source_[current_] != expected) return false;
    current_++;
    column_++;
    return true;
}

Token Lexer::identifier() {
    int startColumn = column_;
    size_t start = current_;
    
    while (!isAtEnd() && (std::isalnum(source_[current_]) || source_[current_] == '_')) {
        advance();
    }
    
    std::string lexeme = source_.substr(start, current_ - start);
    TokenType type = getKeywordType(lexeme);
    
    return Token(type, lexeme, line_, startColumn);
}

Token Lexer::number() {
    int startColumn = column_;
    size_t start = current_;
    
    while (!isAtEnd() && std::isdigit(source_[current_])) {
        advance();
    }
    
    // Check for decimal point
    if (!isAtEnd() && source_[current_] == '.' && std::isdigit(peekNext())) {
        advance(); // consume '.'
        while (!isAtEnd() && std::isdigit(source_[current_])) {
            advance();
        }
    }
    
    std::string lexeme = source_.substr(start, current_ - start);
    return Token(TokenType::NUMBER, lexeme, line_, startColumn);
}

Token Lexer::string(char quote) {
    int startColumn = column_ - 1;
    size_t start = current_;
    
    while (!isAtEnd() && source_[current_] != quote) {
        if (source_[current_] == '\n') {
            line_++;
            column_ = 0;
        }
        advance();
    }
    
    if (isAtEnd()) {
        return Token(TokenType::ERROR, "Unterminated string", line_, startColumn);
    }
    
    std::string lexeme = source_.substr(start, current_ - start);
    advance(); // closing quote
    
    return Token(TokenType::STRING_LITERAL, lexeme, line_, startColumn);
}

Token Lexer::unquotedLiteral() {
    int startColumn = column_;
    size_t start = current_;
    
    while (!isAtEnd() && !std::isspace(source_[current_]) && 
           source_[current_] != ';' && source_[current_] != ',') {
        advance();
    }
    
    std::string lexeme = source_.substr(start, current_ - start);
    return Token(TokenType::UNQUOTED_LITERAL, lexeme, line_, startColumn);
}

} // namespace CHTL
