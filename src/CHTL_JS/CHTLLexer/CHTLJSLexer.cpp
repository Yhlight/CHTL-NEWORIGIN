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
    if (current_char == '[') { position++; return {CHTLJSTokenType::OpenBracket, "["}; }
    if (current_char == '=') { position++; return {CHTLJSTokenType::Equals, "="}; }
    if (current_char == ']') { position++; return {CHTLJSTokenType::CloseBracket, "]"}; }
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

    // Reserved Placeholders
    const std::string placeholder_prefix = "__CHTL_RESERVED_PLACEHOLDER__{";
    if (source.substr(position, placeholder_prefix.length()) == placeholder_prefix) {
        size_t start_pos = position;
        size_t end_pos = source.find("}__", start_pos);
        if (end_pos != std::string::npos) {
            end_pos += 3; // include the "}__" in the token
            std::string placeholder_value = source.substr(start_pos, end_pos - start_pos);
            position = end_pos;
            return {CHTLJSTokenType::ReservedPlaceholder, placeholder_value};
        }
    }

    // Identifiers and Keywords
    if (isalpha(source[position]) || source[position] == '_' || source[position] == '.' || source[position] == '#' || source[position] == '&') {
        size_t start = position;
        // More selective character inclusion for identifiers/selectors.
        // It consumes until it hits a character that cannot be part of an identifier or selector.
        while (position < source.length()) {
            char c = source[position];
            if (!isalnum(c) && c != '_' && c != '-' && c != '.' && c != '#' && c != '&') {
                break; // Break on characters that are definitive separators
            }
            position++;
        }

        std::string word = source.substr(start, position - start);

        if (word == "_JS_CODE_PLACEHOLDER_") return {CHTLJSTokenType::Placeholder, word};
        if (word == "ScriptLoader") return {CHTLJSTokenType::ScriptLoader, word};
        if (word == "Vir") return {CHTLJSTokenType::Vir, word};
        if (word == "Router") return {CHTLJSTokenType::Router, word};

        // All other words, including selectors like ".my-button", are treated as identifiers.
        if (!word.empty()) {
            return {CHTLJSTokenType::Identifier, word};
        }
    }

    // If no token was matched, it's unexpected
    if (position < source.length()) {
        position++;
        return {CHTLJSTokenType::Unexpected, source.substr(position - 1, 1)};
    }

    return {CHTLJSTokenType::EndOfFile, ""};
}