#include "CHTLJSLexer.h"

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
    if (position >= source.length()) {
        return {CHTLJSTokenType::EndOfFile, ""};
    }

    // Accumulate raw JS until a CHTL JS token is found
    size_t start = position;
    while (position < source.length()) {
        if (peek() == '{' && peek(1) == '{') {
            if (position > start) {
                // Return the accumulated RawJS token first
                std::string raw_js = source.substr(start, position - start);
                return {CHTLJSTokenType::RawJS, raw_js};
            }
            advance(); advance();
            return {CHTLJSTokenType::OpenDoubleBrace, "{{"};
        }
        if (peek() == '}' && peek(1) == '}') {
             if (position > start) {
                std::string raw_js = source.substr(start, position - start);
                return {CHTLJSTokenType::RawJS, raw_js};
            }
            advance(); advance();
            return {CHTLJSTokenType::CloseDoubleBrace, "}}"};
        }
        if (peek() == '-' && peek(1) == '>') {
             if (position > start) {
                std::string raw_js = source.substr(start, position - start);
                return {CHTLJSTokenType::RawJS, raw_js};
            }
            advance(); advance();
            return {CHTLJSTokenType::Arrow, "->"};
        }
        advance();
    }

    // If we reached the end, return any remaining RawJS
    if (position > start) {
        return {CHTLJSTokenType::RawJS, source.substr(start, position - start)};
    }

    return {CHTLJSTokenType::EndOfFile, ""};
}