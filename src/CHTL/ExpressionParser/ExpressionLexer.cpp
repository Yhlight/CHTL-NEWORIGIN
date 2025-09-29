#include "ExpressionLexer.h"

ExpressionLexer::ExpressionLexer(const std::string& source)
    : source(source), position(0) {}

std::vector<ExpressionToken> ExpressionLexer::tokenize() {
    std::vector<ExpressionToken> tokens;
    while (!isAtEnd()) {
        tokens.push_back(getNextToken());
    }
    tokens.push_back({ExpressionTokenType::EndOfFile, ""});
    return tokens;
}

ExpressionToken ExpressionLexer::getNextToken() {
    skipWhitespace();

    if (isAtEnd()) {
        return {ExpressionTokenType::EndOfFile, ""};
    }

    char c = advance();

    if (isdigit(c)) {
        return number();
    }
    if (isalpha(c) || c == '_') {
        return identifier();
    }
    if (c == '\'' || c == '\"') {
        return stringLiteral(c);
    }

    switch (c) {
        case '+': return {ExpressionTokenType::Plus, "+"};
        case '-': return {ExpressionTokenType::Minus, "-"};
        case '/': return {ExpressionTokenType::Slash, "/"};
        case '%': return {ExpressionTokenType::Percent, "%"};
        case '(': return {ExpressionTokenType::OpenParen, "("};
        case ')': return {ExpressionTokenType::CloseParen, ")"};
        case '.': return {ExpressionTokenType::Dot, "."};
        case '?': return {ExpressionTokenType::QuestionMark, "?"};
        case ':': return {ExpressionTokenType::Colon, ":"};
        case '*':
            if (peek() == '*') {
                advance();
                return {ExpressionTokenType::DoubleAsterisk, "**"};
            }
            return {ExpressionTokenType::Asterisk, "*"};
        case '&':
            if (peek() == '&') {
                advance();
                return {ExpressionTokenType::LogicalAnd, "&&"};
            }
            break; // Unexpected single '&'
        case '|':
            if (peek() == '|') {
                advance();
                return {ExpressionTokenType::LogicalOr, "||"};
            }
            break; // Unexpected single '|'
        case '=':
            if (peek() == '=') {
                advance();
                return {ExpressionTokenType::EqualsEquals, "=="};
            }
            break; // Unexpected single '='
        case '!':
            if (peek() == '=') {
                advance();
                return {ExpressionTokenType::NotEquals, "!="};
            }
            break; // Unexpected single '!'
        case '>':
            if (peek() == '=') {
                advance();
                return {ExpressionTokenType::GreaterThanEquals, ">="};
            }
            return {ExpressionTokenType::GreaterThan, ">"};
        case '<':
            if (peek() == '=') {
                advance();
                return {ExpressionTokenType::LessThanEquals, "<="};
            }
            return {ExpressionTokenType::LessThan, "<"};
    }

    return {ExpressionTokenType::Unexpected, std::string(1, c)};
}

bool ExpressionLexer::isAtEnd() const {
    return position >= source.length();
}

char ExpressionLexer::peek() const {
    if (isAtEnd()) return '\0';
    return source[position];
}

char ExpressionLexer::advance() {
    return source[position++];
}

void ExpressionLexer::skipWhitespace() {
    while (!isAtEnd() && isspace(peek())) {
        advance();
    }
}

ExpressionToken ExpressionLexer::number() {
    std::string value;
    size_t startPos = position - 1;
    while (!isAtEnd() && (isdigit(peek()) || peek() == '.')) {
        value += advance();
    }
    // Also capture potential units that are attached directly to the number
    while (!isAtEnd() && (isalpha(peek()) || peek() == '%')) {
        value += advance();
    }
    return {ExpressionTokenType::Number, source.substr(startPos, position - startPos)};
}

ExpressionToken ExpressionLexer::identifier() {
    std::string value;
    size_t startPos = position - 1;
    while (!isAtEnd() && (isalnum(peek()) || peek() == '_')) {
        value += advance();
    }
    return {ExpressionTokenType::Identifier, source.substr(startPos, position - startPos)};
}

ExpressionToken ExpressionLexer::stringLiteral(char quoteType) {
    std::string value;
    while (!isAtEnd() && peek() != quoteType) {
        value += advance();
    }
    advance(); // Consume the closing quote
    return {ExpressionTokenType::String, value};
}