#include "chtl/Lexer.hpp"
#include <cctype>

namespace chtl {

Lexer::Lexer(std::string_view input) : source_(input) {}

char Lexer::current() const {
    if (index_ >= source_.size()) return '\0';
    return source_[index_];
}

char Lexer::advance() {
    char c = current();
    if (c == '\0') return c;
    ++index_;
    if (c == '\n') {
        ++loc_.line;
        loc_.column = 1;
    } else {
        ++loc_.column;
    }
    return c;
}

bool Lexer::match(char expected) {
    if (current() != expected) return false;
    advance();
    return true;
}

void Lexer::skipWhitespaceAndComments() {
    for (;;) {
        while (std::isspace(static_cast<unsigned char>(current()))) advance();
        // line comment: //...
        if (current() == '/' && index_ + 1 < source_.size() && source_[index_ + 1] == '/') {
            while (current() != '\0' && current() != '\n') advance();
            continue;
        }
        break;
    }
}

Token Lexer::make(TokenKind kind, size_t startIndex, SourceLocation startLoc) {
    Token t;
    t.kind = kind;
    t.lexeme.assign(source_.data() + startIndex, index_ - startIndex);
    t.range.begin = startLoc;
    t.range.end = loc_;
    return t;
}

Token Lexer::lexIdentifier(size_t startIndex, SourceLocation startLoc) {
    while (std::isalnum(static_cast<unsigned char>(current())) || current() == '_' || current() == '-') advance();
    return make(TokenKind::Identifier, startIndex, startLoc);
}

Token Lexer::lexString(size_t startIndex, SourceLocation startLoc) {
    char quote = source_[startIndex];
    // consume opening quote already included; advance until closing
    while (current() != '\0' && current() != quote) {
        if (current() == '\\') {
            advance(); // skip escape
            if (current() != '\0') advance();
        } else {
            advance();
        }
    }
    if (current() == quote) advance();
    return make(TokenKind::String, startIndex, startLoc);
}

Token Lexer::lexNumber(size_t startIndex, SourceLocation startLoc) {
    while (std::isdigit(static_cast<unsigned char>(current()))) advance();
    if (current() == '.') {
        advance();
        while (std::isdigit(static_cast<unsigned char>(current()))) advance();
    }
    return make(TokenKind::Number, startIndex, startLoc);
}

Token Lexer::lexTextUntil(char terminal) {
    size_t startIndex = index_;
    SourceLocation startLoc = loc_;
    while (current() != '\0' && current() != terminal) advance();
    return make(TokenKind::Text, startIndex, startLoc);
}

Token Lexer::next() {
    if (hasLookahead_) {
        hasLookahead_ = false;
        return lookahead_;
    }

    skipWhitespaceAndComments();

    size_t start = index_;
    SourceLocation sLoc = loc_;
    char c = advance();

    switch (c) {
        case '\0': return make(TokenKind::EndOfFile, start, sLoc);
        case '{': return make(TokenKind::LBrace, start, sLoc);
        case '}': return make(TokenKind::RBrace, start, sLoc);
        case '(': return make(TokenKind::LParen, start, sLoc);
        case ')': return make(TokenKind::RParen, start, sLoc);
        case '[': return make(TokenKind::LBracket, start, sLoc);
        case ']': return make(TokenKind::RBracket, start, sLoc);
        case '<': return make(TokenKind::Less, start, sLoc);
        case '>': return make(TokenKind::Greater, start, sLoc);
        case '/': return make(TokenKind::Slash, start, sLoc);
        case ':': return make(TokenKind::Colon, start, sLoc);
        case '=': return make(TokenKind::Equal, start, sLoc);
        case ';': return make(TokenKind::Semicolon, start, sLoc);
        case ',': return make(TokenKind::Comma, start, sLoc);
        case '.': return make(TokenKind::Dot, start, sLoc);
        case '#': return make(TokenKind::Hash, start, sLoc);
        case '@': return make(TokenKind::At, start, sLoc);
        case '"':
        case '\'':
            return lexString(start, sLoc);
        default:
            if (std::isalpha(static_cast<unsigned char>(c)) || c == '_' || c == '-') {
                while (std::isalnum(static_cast<unsigned char>(current())) || current() == '_' || current() == '-') advance();
                return make(TokenKind::Identifier, start, sLoc);
            }
            if (std::isdigit(static_cast<unsigned char>(c))) {
                while (std::isdigit(static_cast<unsigned char>(current()))) advance();
                if (current() == '.') { advance(); while (std::isdigit(static_cast<unsigned char>(current()))) advance(); }
                return make(TokenKind::Number, start, sLoc);
            }
            // raw text until special
            return make(TokenKind::Text, start, sLoc);
    }
}

const Token& Lexer::peek() {
    if (!hasLookahead_) {
        lookahead_ = next();
        hasLookahead_ = true;
    }
    return lookahead_;
}

bool Lexer::isAtEnd() const {
    return index_ >= source_.size();
}

} // namespace chtl
