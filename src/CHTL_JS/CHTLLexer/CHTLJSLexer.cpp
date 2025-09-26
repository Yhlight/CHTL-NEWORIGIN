#include "CHTLJSLexer.h"
#include <cctype>
#include <unordered_set>

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

    size_t start = position;

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

    // Keywords or RawJS
    const static std::unordered_set<std::string> keywords = {"Listen", "Delegate", "Animate"};

    // Check for a keyword
    if (isalpha(source[start])) {
        size_t end = start;
        while (end < source.length() && isalnum(source[end])) {
            end++;
        }
        std::string word = source.substr(start, end - start);
        if (keywords.count(word)) {
            position = end;
            return {CHTLJSTokenType::Identifier, word};
        }
    }


    // If it's not a keyword or special token, consume as RawJS
    while (position < source.length()) {
        // Stop if we hit whitespace that might delimit a token
        if (isspace(source[position])) {
            break;
        }
        // Stop if we hit the start of any other token
        if (source.substr(position, 3) == "&->" ||
            source.substr(position, 2) == "->" ||
            source.substr(position, 2) == "{{" ||
            source.substr(position, 2) == "}}" ||
            source[position] == '{' || source[position] == '}' ||
            source[position] == ':' || source[position] == ',') {
            break;
        }

        // Lookahead to see if the next word is a keyword
        if (isalpha(source[position])) {
            size_t end = position;
            while (end < source.length() && isalnum(source[end])) {
                end++;
            }
            std::string word = source.substr(position, end - position);
            if (keywords.count(word)) {
                break;
            }
        }

        position++;
    }

    std::string value = source.substr(start, position - start);

    if (!value.empty()) {
        return {CHTLJSTokenType::RawJS, value};
    }

    // If we consumed nothing, it means we're at a token, so retry
    return getNextToken();
}