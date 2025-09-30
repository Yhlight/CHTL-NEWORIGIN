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
    while (isalnum(peek()) || peek() == '_') {
        value += advance();
    }

    if (keywords.count(value)) {
        return {keywords.at(value), value, line, startCol};
    }

    return {CHTLJSTokenType::Identifier, value, line, startCol};
}

CHTLJSToken CHTLJSLexer::stringLiteral(char quoteType) {
    std::string value;
    int startCol = column;
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
    return {CHTLJSTokenType::String, value, line, startCol};
}

CHTLJSToken CHTLJSLexer::getNextToken() {
    skipWhitespace();

    if (position >= source.length()) {
        return {CHTLJSTokenType::EndOfFile, "", line, column};
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
        return {CHTLJSTokenType::Number, value, line, startCol};
    }
    if (current == '"' || current == '\'') {
        return stringLiteral(current);
    }

    switch (current) {
        case '{':
            advance();
            if (peek() == '{') {
                advance();
                return {CHTLJSTokenType::OpenDoubleBrace, "{{", line, startCol};
            }
            return {CHTLJSTokenType::OpenBrace, "{", line, startCol};
        case '}':
            advance();
            if (peek() == '}') {
                advance();
                return {CHTLJSTokenType::CloseDoubleBrace, "}}", line, startCol};
            }
            return {CHTLJSTokenType::CloseBrace, "}", line, startCol};
        case '-':
            if (position + 1 < source.length() && source[position + 1] == '>') {
                advance(); advance();
                return {CHTLJSTokenType::RightArrow, "->", line, startCol};
            }
            break;
        case '&':
            if (position + 2 < source.length() && source[position + 1] == '-' && source[position + 2] == '>') {
                advance(); advance(); advance();
                return {CHTLJSTokenType::EventBind, "&->", line, startCol};
            }
            break;
        case '(': advance(); return {CHTLJSTokenType::OpenParen, "(", line, startCol};
        case ')': advance(); return {CHTLJSTokenType::CloseParen, ")", line, startCol};
        case '[': advance(); return {CHTLJSTokenType::OpenBracket, "[", line, startCol};
        case ']': advance(); return {CHTLJSTokenType::CloseBracket, "]", line, startCol};
        case ':': advance(); return {CHTLJSTokenType::Colon, ":", line, startCol};
        case ';': advance(); return {CHTLJSTokenType::Semicolon, ";", line, startCol};
        case ',': advance(); return {CHTLJSTokenType::Comma, ",", line, startCol};
        case '.': advance(); return {CHTLJSTokenType::Dot, ".", line, startCol};
    }

    // As a fallback for things like JS code inside callbacks, we consume until a known CHTL JS delimiter
    // This is a simplification. A truly robust parser would need to handle nested structures.
    std::string rawJs;
    int brace_level = 0;

    // If we are inside an event handler, e.g., after a colon.
    size_t last_token_end = position;
    if (position > 0) {
        char prev_char = source[position - 1];
        if (isspace(prev_char)) {
             size_t temp_pos = position - 2;
             while(temp_pos > 0 && isspace(source[temp_pos])) {
                 temp_pos--;
             }
             prev_char = source[temp_pos];
        }
        if (prev_char == ':') {
             brace_level = 1;
        }
    }


    while (position < source.length()) {
        char c = peek();
        if (c == '{') brace_level++;
        if (c == '}') brace_level--;

        // Stop if we hit a closing brace at the top level or a comma between event handlers
        if ((c == '}' && brace_level < 0) || (c == ',' && brace_level == 0)) {
            break;
        }
        rawJs += advance();
    }

    if (!rawJs.empty()) {
        return {CHTLJSTokenType::RawJavaScript, rawJs, line, startCol};
    }


    return {CHTLJSTokenType::Unexpected, std::string(1, advance()), line, startCol};
}