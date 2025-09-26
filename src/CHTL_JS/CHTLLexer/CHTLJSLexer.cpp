#include "CHTLJSLexer.h"
#include <cctype>

CHTLJSLexer::CHTLJSLexer(const std::string& source) : source(source) {}

CHTLJSToken CHTLJSLexer::makeToken(CHTLJSTokenType type, const std::string& value) {
    return {type, value};
}

CHTLJSToken CHTLJSLexer::makeToken(CHTLJSTokenType type, char value) {
    return {type, std::string(1, value)};
}

std::vector<CHTLJSToken> CHTLJSLexer::tokenize() {
    std::vector<CHTLJSToken> tokens;
    std::string raw_buffer;

    auto flush_raw_buffer = [this, &raw_buffer, &tokens]() {
        if (!raw_buffer.empty()) {
            // Check if the buffer is a known keyword
            if (raw_buffer == "Listen" || raw_buffer == "Delegate" || raw_buffer == "Animate") {
                tokens.push_back(this->makeToken(CHTLJSTokenType::Identifier, raw_buffer));
            } else {
                 // Check if it could be an event name or property name
                bool is_identifier = true;
                if (raw_buffer.empty()) {
                    is_identifier = false;
                } else {
                    for (char c : raw_buffer) {
                        if (!isalnum(c) && c != '_') {
                            is_identifier = false;
                            break;
                        }
                    }
                }

                if (is_identifier) {
                     tokens.push_back(this->makeToken(CHTLJSTokenType::Identifier, raw_buffer));
                } else {
                     tokens.push_back(this->makeToken(CHTLJSTokenType::RawJS, raw_buffer));
                }
            }
            raw_buffer.clear();
        }
    };

    while (position < source.length()) {
        char current = source[position];
        char next = (position + 1 < source.length()) ? source[position + 1] : '\0';

        if (current == '-' && next == '>') {
            flush_raw_buffer();
            tokens.push_back(this->makeToken(CHTLJSTokenType::Arrow, "->"));
            position += 2;
            continue;
        }
        if (current == '&' && next == '-' && (position + 2 < source.length()) && source[position + 2] == '>') {
            flush_raw_buffer();
            tokens.push_back(this->makeToken(CHTLJSTokenType::EventBindingOperator, "&->"));
            position += 3;
            continue;
        }
        if (current == '{' && next == '{') {
            flush_raw_buffer();
            tokens.push_back(this->makeToken(CHTLJSTokenType::OpenDoubleBrace, "{{"));
            position += 2;
            continue;
        }
        if (current == '}' && next == '}') {
            flush_raw_buffer();
            tokens.push_back(this->makeToken(CHTLJSTokenType::CloseDoubleBrace, "}}"));
            position += 2;
            continue;
        }
        if (current == '{') {
            flush_raw_buffer();
            tokens.push_back(this->makeToken(CHTLJSTokenType::OpenBrace, "{"));
            position++;
            continue;
        }
        if (current == '}') {
            flush_raw_buffer();
            tokens.push_back(this->makeToken(CHTLJSTokenType::CloseBrace, "}"));
            position++;
            continue;
        }
        if (current == ':') {
            flush_raw_buffer();
            tokens.push_back(this->makeToken(CHTLJSTokenType::Colon, ":"));
            position++;
            continue;
        }
        if (current == ',') {
            flush_raw_buffer();
            tokens.push_back(this->makeToken(CHTLJSTokenType::Comma, ","));
            position++;
            continue;
        }
        if (current == '"') {
            flush_raw_buffer();
            raw_buffer += current; // Add opening quote
            position++;
            while (position < source.length()) {
                raw_buffer += source[position];
                if (source[position] == '"' && source[position - 1] != '\\') {
                    position++;
                    break;
                }
                position++;
            }
            flush_raw_buffer();
            continue;
        }
        if (isspace(current)) {
             flush_raw_buffer();
             // Consume all whitespace
             while(position < source.length() && isspace(source[position])) {
                 position++;
             }
             raw_buffer += ' '; // Preserve a single space to separate tokens
             continue;
        }

        // If it's not a special character, add to the raw buffer
        raw_buffer += current;
        position++;
    }

    flush_raw_buffer(); // Flush any remaining content in the buffer

    tokens.push_back(this->makeToken(CHTLJSTokenType::EndOfFile, ""));
    return tokens;
}