#include "CHTLJSLexer.h"
#include <cctype>

CHTLJSLexer::CHTLJSLexer(const std::string& source)
    : source(source), position(0), line(1), column(1) {
    keywords["Listen"] = CHTLJSTokenType::Listen;
    keywords["Delegate"] = CHTLJSTokenType::Delegate;
    keywords["Animate"] = CHTLJSTokenType::Animate;
    keywords["Router"] = CHTLJSTokenType::Router;
    keywords["Vir"] = CHTLJSTokenType::Vir;
    keywords["ScriptLoader"] = CHTLJSTokenType::ScriptLoader;
    keywords["target"] = CHTLJSTokenType::Target;
    keywords["load"] = CHTLJSTokenType::Load;
}

std::vector<CHTLJSToken> CHTLJSLexer::tokenize() {
    std::vector<CHTLJSToken> tokens;
    CHTLJSToken token = getNextToken();
    while (token.type != CHTLJSTokenType::EndOfFile) {
        tokens.push_back(token);
        token = getNextToken();
    }
    tokens.push_back(token); // Add EOF token
    return tokens;
}

char CHTLJSLexer::peek() {
    if (position >= source.length()) {
        return '\0';
    }
    return source[position];
}

char CHTLJSLexer::advance() {
    if (position >= source.length()) {
        return '\0';
    }
    char current = source[position];
    position++;
    if (current == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return current;
}

void CHTLJSLexer::skipWhitespace() {
    while (position < source.length() && isspace(peek())) {
        advance();
    }
}

CHTLJSToken CHTLJSLexer::identifier() {
    std::string value;
    int startCol = column;
    size_t startPos = position;
    while (isalnum(peek()) || peek() == '_') {
        value += advance();
    }

    if (keywords.count(value)) {
        return {keywords.at(value), value, line, startCol, startPos};
    }

    return {CHTLJSTokenType::Identifier, value, line, startCol, startPos};
}

CHTLJSToken CHTLJSLexer::stringLiteral(char quoteType) {
    std::string value;
    int startCol = column;
    size_t startPos = position;
    advance(); // consume opening quote
    while (peek() != quoteType && peek() != '\0') {
        if (peek() == '\\') { // Handle escape sequences
            value += advance();
        }
        value += advance();
    }
    if (peek() == quoteType) {
        advance(); // consume closing quote
    }
    return {CHTLJSTokenType::String, value, line, startCol, startPos};
}

CHTLJSToken CHTLJSLexer::getNextToken() {
    skipWhitespace();

    size_t startPos = position;

    if (position >= source.length()) {
        return {CHTLJSTokenType::EndOfFile, "", line, column, startPos};
    }

    char current = peek();
    int startCol = column;

    if (isalpha(current) || current == '_') {
        return identifier();
    }
    if (isdigit(current)) {
        std::string value;
        while(isdigit(peek())) {
            value += advance();
        }
        return {CHTLJSTokenType::Number, value, line, startCol, startPos};
    }
    if (current == '"' || current == '\'') {
        return stringLiteral(current);
    }

    switch (current) {
        case '{':
            advance();
            if (peek() == '{') {
                advance();
                return {CHTLJSTokenType::OpenDoubleBrace, "{{", line, startCol, startPos};
            }
            return {CHTLJSTokenType::OpenBrace, "{", line, startCol, startPos};
        case '}':
            advance();
            if (peek() == '}') {
                advance();
                return {CHTLJSTokenType::CloseDoubleBrace, "}}", line, startCol, startPos};
            }
            return {CHTLJSTokenType::CloseBrace, "}", line, startCol, startPos};
        case '-':
            if (position + 1 < source.length() && source[position + 1] == '>') {
                advance(); advance();
                return {CHTLJSTokenType::RightArrow, "->", line, startCol, startPos};
            }
            break;
        case '&':
            if (position + 2 < source.length() && source[position + 1] == '-' && source[position + 2] == '>') {
                advance(); advance(); advance();
                return {CHTLJSTokenType::EventBind, "&->", line, startCol, startPos};
            }
            break;
        case '(': advance(); return {CHTLJSTokenType::OpenParen, "(", line, startCol, startPos};
        case ')': advance(); return {CHTLJSTokenType::CloseParen, ")", line, startCol, startPos};
        case '[': advance(); return {CHTLJSTokenType::OpenBracket, "[", line, startCol, startPos};
        case ']': advance(); return {CHTLJSTokenType::CloseBracket, "]", line, startCol, startPos};
        case ':': advance(); return {CHTLJSTokenType::Colon, ":", line, startCol, startPos};
        case ';': advance(); return {CHTLJSTokenType::Semicolon, ";", line, startCol, startPos};
        case ',': advance(); return {CHTLJSTokenType::Comma, ",", line, startCol, startPos};
        case '.': advance(); return {CHTLJSTokenType::Dot, ".", line, startCol, startPos};
    }

    // Fallback for RawJavaScript
    std::string rawJs;
    int brace_level = 0;

    size_t temp_pos = position > 0 ? position -1 : 0;
    while(temp_pos > 0 && isspace(source[temp_pos])) {
        temp_pos--;
    }
    if (source[temp_pos] == ':') {
         brace_level = 1;
    }

    while (position < source.length()) {
        char c = peek();
        if (c == '{') brace_level++;
        if (c == '}') brace_level--;

        if ((c == '}' && brace_level < 0) || (c == ',' && brace_level <= 0)) {
            size_t next_non_space = source.find_first_not_of(" \t\n\r", position + 1);
            if (c == ',' && next_non_space != std::string::npos && isalpha(source[next_non_space])) {
                 break;
            }
        }
        rawJs += advance();
    }

    if (!rawJs.empty()) {
        return {CHTLJSTokenType::RawJavaScript, rawJs, line, startCol, startPos};
    }


    return {CHTLJSTokenType::Unexpected, std::string(1, advance()), line, startCol, startPos};
}