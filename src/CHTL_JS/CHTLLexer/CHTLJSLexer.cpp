#include "CHTLJSLexer.h"
#include <cctype>

CHTLJSLexer::CHTLJSLexer(const std::string& source) : source(source) {}

char CHTLJSLexer::peek(int offset) const {
    if (position + offset >= source.length()) {
        return '\0';
    }
    return source[position + offset];
}

char CHTLJSLexer::advance() {
    if (position >= source.length()) {
        return '\0';
    }
    return source[position++];
}

std::vector<CHTLJSToken> CHTLJSLexer::tokenize() {
    std::vector<CHTLJSToken> tokens;
    CHTLJSToken token;
    do {
        token = getNextToken();
        tokens.push_back(token);
    } while (token.type != CHTLJSTokenType::EndOfFile);
    return tokens;
}

CHTLJSToken CHTLJSLexer::getNextToken() {
    while (position < source.length() && isspace(peek())) {
        advance();
    }

    if (position >= source.length()) {
        return {CHTLJSTokenType::EndOfFile, ""};
    }

    // CHTL JS specific operators
    if (peek() == '{' && peek(1) == '{') { advance(); advance(); return {CHTLJSTokenType::OpenDoubleBrace, "{{"}; }
    if (peek() == '}' && peek(1) == '}') { advance(); advance(); return {CHTLJSTokenType::CloseDoubleBrace, "}}"}; }
    if (peek() == '-' && peek(1) == '>') { advance(); advance(); return {CHTLJSTokenType::Arrow, "->"}; }
    if (peek() == '&' && peek(1) == '-' && peek(2) == '>') { advance(); advance(); advance(); return {CHTLJSTokenType::EventBindingOperator, "&->"}; }

    // Punctuation for Listen blocks
    if (peek() == '{') { advance(); return {CHTLJSTokenType::OpenBrace, "{"}; }
    if (peek() == '}') { advance(); return {CHTLJSTokenType::CloseBrace, "}"}; }
    if (peek() == ':') { advance(); return {CHTLJSTokenType::Colon, ":"}; }
    if (peek() == ',') { advance(); return {CHTLJSTokenType::Comma, ","}; }

    // Identifiers (like 'Listen') or Raw JS
    size_t start = position;
    if (isalpha(peek()) || peek() == '_') {
        while (isalnum(peek()) || peek() == '_') {
            advance();
        }
        std::string value = source.substr(start, position - start);
        if (value == "Listen") {
            return {CHTLJSTokenType::Identifier, value};
        }
        // If not a keyword, treat it as part of a raw JS block
        position = start; // backtrack
    }

    // Default to consuming as RawJS until a CHTL JS token is found
    while (position < source.length()) {
        if ((peek() == '{' && peek(1) == '{') ||
            (peek() == '}' && peek(1) == '}') ||
            (peek() == '-' && peek(1) == '>') ||
            (peek() == '&' && peek(1) == '-' && peek(2) == '>') ||
            (peek() == '{') || (peek() == '}') || (peek() == ':') || (peek() == ','))
        {
            break;
        }
        advance();
    }

    if (position > start) {
        return {CHTLJSTokenType::RawJS, source.substr(start, position - start)};
    }

    return {CHTLJSTokenType::Unexpected, std::string(1, peek())};
}