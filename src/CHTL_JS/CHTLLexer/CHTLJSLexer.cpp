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
    // Skip whitespace
    while (position < source.length() && isspace(source[position])) {
        position++;
    }

    if (position >= source.length()) {
        return {CHTLJSTokenType::EndOfFile, ""};
    }

    // Check for multi-char operators first
    if (source.substr(position, 3) == "&->") {
        position += 3;
        return {CHTLJSTokenType::EventBindingOperator, "&->"};
    }
    if (source.substr(position, 2) == "->") {
        position += 2;
        return {CHTLJSTokenType::Arrow, "->"};
    }
    if (source.substr(position, 2) == "{{") {
        position += 2;
        return {CHTLJSTokenType::OpenDoubleBrace, "{{"};
    }
    if (source.substr(position, 2) == "}}") {
        position += 2;
        return {CHTLJSTokenType::CloseDoubleBrace, "}}"};
    }

    // Check for single-char punctuation
    char current_char = source[position];
    if (current_char == '{') { position++; return {CHTLJSTokenType::OpenBrace, "{"}; }
    if (current_char == '}') { position++; return {CHTLJSTokenType::CloseBrace, "}"}; }
    if (current_char == ':') { position++; return {CHTLJSTokenType::Colon, ":"}; }
    if (current_char == ',') { position++; return {CHTLJSTokenType::Comma, ","}; }

    // Check for string literals (for file paths, etc.)
    if (current_char == '"' || current_char == '\'') {
        char quote_type = current_char;
        position++; // consume opening quote
        size_t value_start = position;
        while (position < source.length() && source[position] != quote_type) {
            // Handle escaped quotes if necessary, for now we keep it simple
            position++;
        }
        std::string value = source.substr(value_start, position - value_start);
        if (position < source.length()) {
            position++; // consume closing quote
        }
        return {CHTLJSTokenType::StringLiteral, value};
    }

    // Check for identifiers and keywords
    if (isalpha(source[position])) {
        size_t start = position;
        while (position < source.length() && isalnum(source[position])) {
            position++;
        }
        std::string word = source.substr(start, position - start);

        if (word == "ScriptLoader") {
            return {CHTLJSTokenType::ScriptLoader, word};
        }
        // Other keywords are treated as identifiers for now.
        return {CHTLJSTokenType::Identifier, word};
    }

    // Fallback for raw JS or unquoted literals. This is a greedy consumption.
    size_t start = position;
    while (position < source.length()) {
        if (source.substr(position, 3) == "&->" ||
            source.substr(position, 2) == "->" ||
            source.substr(position, 2) == "{{" ||
            source.substr(position, 2) == "}}" ||
            source[position] == '{' || source[position] == '}' ||
            source[position] == ':' || source[position] == ',') {
            break;
        }

        // Lookahead to check for keywords
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
        // For unquoted paths, this will be tokenized as RawJS.
        // The parser will handle this.
        return {CHTLJSTokenType::RawJS, value};
    }

    // If we get here, something is wrong.
    position++;
    return {CHTLJSTokenType::Unexpected, source.substr(position - 1, 1)};
}