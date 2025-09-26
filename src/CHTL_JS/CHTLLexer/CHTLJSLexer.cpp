#include "CHTLJSLexer.h"
#include <cctype>
#include <unordered_map>

CHTLJSLexer::CHTLJSLexer(const std::string& source) : source(source), position(0) {}

std::vector<CHTLJSToken> CHTLJSLexer::tokenize() {
    std::vector<CHTLJSToken> tokens;
    while (position < source.length()) {
        tokens.push_back(getNextToken());
    }
    tokens.push_back({CHTLJSTokenType::EndOfFile, ""});
    return tokens;
}

CHTLJSToken CHTLJSLexer::getNextToken() {
    while (position < source.length() && isspace(source[position])) {
        position++;
    }

    if (position >= source.length()) {
        return {CHTLJSTokenType::EndOfFile, ""};
    }

    // CHTL JS specific operators
    if (source.substr(position, 3) == "&->") { position += 3; return {CHTLJSTokenType::EventBindingOperator, "&->"}; }
    if (source.substr(position, 2) == "->") { position += 2; return {CHTLJSTokenType::Arrow, "->"}; }
    if (source.substr(position, 2) == "{{") { position += 2; return {CHTLJSTokenType::OpenDoubleBrace, "{{"}; }
    if (source.substr(position, 2) == "}}") { position += 2; return {CHTLJSTokenType::CloseDoubleBrace, "}}"}; }

    // Punctuation
    char current_char = source[position];
    if (current_char == '{') { position++; return {CHTLJSTokenType::OpenBrace, "{"}; }
    if (current_char == '}') { position++; return {CHTLJSTokenType::CloseBrace, "}"}; }
    if (current_char == ':') { position++; return {CHTLJSTokenType::Colon, ":"}; }
    if (current_char == ',') { position++; return {CHTLJSTokenType::Comma, ","}; }
    if (current_char == ';') { position++; return {CHTLJSTokenType::Semicolon, ";"}; }

    // String literals
    if (current_char == '"' || current_char == '\'') {
        char quote_type = current_char;
        position++;
        size_t value_start = position;
        while (position < source.length() && source[position] != quote_type) {
            position++;
        }
        std::string value = source.substr(value_start, position - value_start);
        if (position < source.length()) position++;
        return {CHTLJSTokenType::StringLiteral, value};
    }

    // Keywords
    if (isalpha(source[position])) {
        size_t start = position;
        size_t end = start;
        while (end < source.length() && isalnum(source[end])) {
            end++;
        }
        std::string word = source.substr(start, end - start);
        if (word == "ScriptLoader") {
            position = end;
            return {CHTLJSTokenType::ScriptLoader, word};
        }
        if (word == "Listen" || word == "Delegate" || word == "Animate") {
            position = end;
            return {CHTLJSTokenType::Identifier, word};
        }
    }

    // Fallback for raw JS
    size_t start = position;
    while (position < source.length()) {
        if (source.substr(position, 2) == "{{" || source.substr(position, 2) == "}}" || source.substr(position, 2) == "->" || source.substr(position, 3) == "&->" ||
            source[position] == '{' || source[position] == '}' || source[position] == ':' || source[position] == ',') {
            break;
        }

        if (isalpha(source[position])) {
            size_t end = position;
            while(end < source.length() && isalnum(source[end])) {
                end++;
            }
            std::string word = source.substr(position, end - position);
            if(word == "ScriptLoader" || word == "Listen" || word == "Delegate" || word == "Animate") {
                break;
            }
        }
        position++;
    }

    std::string value = source.substr(start, position - start);
    if (!value.empty()) {
        return {CHTLJSTokenType::RawJS, value};
    }

    // This should not be reached if the logic is correct, but as a safeguard
    if (position < source.length()) {
        position++;
        return {CHTLJSTokenType::Unexpected, source.substr(position - 1, 1)};
    }

    return {CHTLJSTokenType::EndOfFile, ""};
}