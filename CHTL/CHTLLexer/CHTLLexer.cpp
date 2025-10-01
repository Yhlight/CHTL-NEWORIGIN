#include "CHTLLexer.h"
#include <cctype>
#include <unordered_map>

namespace CHTL {

CHTLLexer::CHTLLexer() {}

CHTLLexer::~CHTLLexer() {}

std::vector<Token> CHTLLexer::tokenize(const std::string& input) {
    std::vector<Token> tokens;
    std::string::size_type pos = 0;

    const std::unordered_map<std::string, TokenType> keywords = {
        {"text", TokenType::KEYWORD_TEXT},
        {"style", TokenType::KEYWORD_STYLE},
        {"script", TokenType::KEYWORD_SCRIPT},
        {"inherit", TokenType::KEYWORD_INHERIT},
        {"delete", TokenType::KEYWORD_DELETE},
        {"insert", TokenType::KEYWORD_INSERT},
        {"after", TokenType::KEYWORD_AFTER},
        {"before", TokenType::KEYWORD_BEFORE},
        {"replace", TokenType::KEYWORD_REPLACE},
        {"from", TokenType::KEYWORD_FROM},
        {"as", TokenType::KEYWORD_AS},
        {"except", TokenType::KEYWORD_EXCEPT},
        {"use", TokenType::KEYWORD_USE},
        {"html5", TokenType::KEYWORD_HTML5},
    };

    while (pos < input.length()) {
        if (std::isspace(input[pos])) {
            pos++;
            continue;
        }

        // Comments
        if (pos + 1 < input.length() && input[pos] == '/') {
            if (input[pos + 1] == '/') {
                std::string::size_type comment_start = pos + 2;
                std::string::size_type comment_end = input.find('\n', comment_start);
                tokens.push_back({TokenType::COMMENT, (comment_end == std::string::npos) ? input.substr(comment_start) : input.substr(comment_start, comment_end - comment_start)});
                pos = (comment_end == std::string::npos) ? input.length() : comment_end;
                continue;
            }
            if (input[pos + 1] == '*') {
                std::string::size_type comment_start = pos + 2;
                std::string::size_type comment_end = input.find("*/", comment_start);
                if (comment_end == std::string::npos) {
                    tokens.push_back({TokenType::COMMENT, input.substr(comment_start)});
                    pos = input.length();
                } else {
                    tokens.push_back({TokenType::COMMENT, input.substr(comment_start, comment_end - comment_start)});
                    pos = comment_end + 2;
                }
                continue;
            }
        }
        if (input[pos] == '#') {
            if (pos + 1 < input.length() && input[pos + 1] == ' ') {
                std::string::size_type comment_start = pos + 2;
                std::string::size_type comment_end = input.find('\n', comment_start);
                tokens.push_back({TokenType::COMMENT, (comment_end == std::string::npos) ? input.substr(comment_start) : input.substr(comment_start, comment_end - comment_start)});
                pos = (comment_end == std::string::npos) ? input.length() : comment_end;
            } else {
                tokens.push_back({TokenType::HASH, "#"});
                pos++;
            }
            continue;
        }

        // Punctuation & Operators (longer tokens first)
        if (pos + 2 < input.length() && input[pos] == '&' && input[pos+1] == '-' && input[pos+2] == '>') {
             tokens.push_back({TokenType::EVENT_BIND_OP, "&->"}); pos+=3; continue;
        }
        if (pos + 1 < input.length()) {
            char current_char = input[pos];
            char next_char = input[pos+1];
            if (current_char == '{' && next_char == '{') { tokens.push_back({TokenType::DOUBLE_L_BRACE, "{{"}); pos+=2; continue;}
            if (current_char == '}' && next_char == '}') { tokens.push_back({TokenType::DOUBLE_R_BRACE, "}}"}); pos+=2; continue;}
            if (current_char == '-' && next_char == '>') { tokens.push_back({TokenType::ARROW, "->"}); pos+=2; continue;}
            if (current_char == '&' && next_char == '&') { tokens.push_back({TokenType::DOUBLE_AMPERSAND, "&&"}); pos += 2; continue; }
            if (current_char == '|' && next_char == '|') { tokens.push_back({TokenType::DOUBLE_PIPE, "||"}); pos += 2; continue; }
            if (current_char == '!' && next_char == '=') { tokens.push_back({TokenType::NOT_EQUAL, "!="}); pos += 2; continue; }
            if (current_char == '<' && next_char == '=') { tokens.push_back({TokenType::LESS_EQUAL, "<="}); pos += 2; continue; }
            if (current_char == '>' && next_char == '=') { tokens.push_back({TokenType::GREATER_EQUAL, ">="}); pos += 2; continue; }
            if (current_char == '=' && next_char == '=') { tokens.push_back({TokenType::DOUBLE_EQUAL, "=="}); pos += 2; continue; }
            if (current_char == '*' && next_char == '*') { tokens.push_back({TokenType::STAR_STAR, "**"}); pos += 2; continue; }
            if (current_char == '+' && next_char == '=') { tokens.push_back({TokenType::PLUS_EQUAL, "+="}); pos += 2; continue; }
            if (current_char == '-' && next_char == '=') { tokens.push_back({TokenType::MINUS_EQUAL, "-="}); pos += 2; continue; }
            if (current_char == '*' && next_char == '=') { tokens.push_back({TokenType::STAR_EQUAL, "*="}); pos += 2; continue; }
            if (current_char == '/' && next_char == '=') { tokens.push_back({TokenType::SLASH_EQUAL, "/="}); pos += 2; continue; }
        }

        // Single character tokens
        switch(input[pos]) {
            case '(': tokens.push_back({TokenType::L_PAREN, "("}); pos++; continue;
            case ')': tokens.push_back({TokenType::R_PAREN, ")"}); pos++; continue;
            case ']': tokens.push_back({TokenType::R_BRACKET, "]"}); pos++; continue;
            case ',': tokens.push_back({TokenType::COMMA, ","}); pos++; continue;
            case '.': tokens.push_back({TokenType::DOT, "."}); pos++; continue;
            case '$': tokens.push_back({TokenType::DOLLAR, "$"}); pos++; continue;
            case '{': tokens.push_back({TokenType::L_BRACE, "{"}); pos++; continue;
            case '}': tokens.push_back({TokenType::R_BRACE, "}"}); pos++; continue;
            case ':': tokens.push_back({TokenType::COLON, ":"}); pos++; continue;
            case '=': tokens.push_back({TokenType::EQUAL, "="}); pos++; continue;
            case ';': tokens.push_back({TokenType::SEMICOLON, ";"}); pos++; continue;
            case '&': tokens.push_back({TokenType::AMPERSAND, "&"}); pos++; continue;
            case '!': tokens.push_back({TokenType::EXCLAMATION, "!"}); pos++; continue;
            case '<': tokens.push_back({TokenType::LESS, "<"}); pos++; continue;
            case '>': tokens.push_back({TokenType::GREATER, ">"}); pos++; continue;
            case '+': tokens.push_back({TokenType::PLUS, "+"}); pos++; continue;
            case '-': tokens.push_back({TokenType::MINUS, "-"}); pos++; continue;
            case '*': tokens.push_back({TokenType::STAR, "*"}); pos++; continue;
            case '/': tokens.push_back({TokenType::SLASH, "/"}); pos++; continue;
            case '%': tokens.push_back({TokenType::PERCENT, "%"}); pos++; continue;
            case '?': tokens.push_back({TokenType::QUESTION, "?"}); pos++; continue;
        }

        // Keywords and special constructs
        if (input[pos] == '@') {
            if (pos + 1 < input.length() && std::isalpha(input[pos + 1])) {
                std::string::size_type type_start = pos + 1;
                pos++;
                while (pos < input.length() && std::isalnum(input[pos])) {
                    pos++;
                }
                std::string value = input.substr(type_start, pos - type_start);
                if (value == "Style") { tokens.push_back({TokenType::TYPE_STYLE, value}); }
                else if (value == "Element") { tokens.push_back({TokenType::TYPE_ELEMENT, value}); }
                else if (value == "Var") { tokens.push_back({TokenType::TYPE_VAR, value}); }
                else if (value == "Html") { tokens.push_back({TokenType::TYPE_HTML, value}); }
                else if (value == "JavaScript") { tokens.push_back({TokenType::TYPE_JAVASCRIPT, value}); }
                else if (value == "Chtl") { tokens.push_back({TokenType::TYPE_CHTL, value}); }
                else if (value == "Config") { tokens.push_back({TokenType::TYPE_CONFIG, value}); }
                else if (value == "CJmod") { tokens.push_back({TokenType::TYPE_CJMOD, value}); }
                else { tokens.push_back({TokenType::UNKNOWN, "@" + value}); }
            } else {
                tokens.push_back({TokenType::AT, "@"});
                pos++;
            }
            continue;
        }

        if (input[pos] == '[') {
            std::string::size_type keyword_start = pos;
            std::string::size_type keyword_end = input.find(']', keyword_start);
            if (keyword_end != std::string::npos) {
                std::string value = input.substr(keyword_start, keyword_end - keyword_start + 1);
                if (value == "[Custom]") { tokens.push_back({TokenType::KEYWORD_CUSTOM, value}); }
                else if (value == "[Template]") { tokens.push_back({TokenType::KEYWORD_TEMPLATE, value}); }
                else if (value == "[Origin]") { tokens.push_back({TokenType::KEYWORD_ORIGIN, value}); }
                else if (value == "[Import]") { tokens.push_back({TokenType::KEYWORD_IMPORT, value}); }
                else if (value == "[Namespace]") { tokens.push_back({TokenType::KEYWORD_NAMESPACE, value}); }
                else if (value == "[Configuration]") { tokens.push_back({TokenType::KEYWORD_CONFIGURATION, value}); }
                else if (value == "[Info]") { tokens.push_back({TokenType::KEYWORD_INFO, value}); }
                else if (value == "[Export]") { tokens.push_back({TokenType::KEYWORD_EXPORT, value}); }
                else if (value == "[OriginType]") { tokens.push_back({TokenType::KEYWORD_ORIGIN_TYPE, value}); }
                else if (value == "[Name]") { tokens.push_back({TokenType::KEYWORD_NAME, value}); }
                else {
                    tokens.push_back({TokenType::L_BRACKET, "["});
                    pos++;
                    continue;
                }
                pos = keyword_end + 1;
                continue;
            } else {
                tokens.push_back({TokenType::L_BRACKET, "["});
                pos++;
                continue;
            }
        }

        // Literals
        if (input[pos] == '"' || input[pos] == '\'') {
            char quote_type = input[pos];
            std::string::size_type literal_start = pos + 1;
            std::string::size_type literal_end = input.find(quote_type, literal_start);
            if (literal_end == std::string::npos) {
                tokens.push_back({TokenType::UNKNOWN, input.substr(pos)});
                pos = input.length();
            } else {
                tokens.push_back({TokenType::STRING_LITERAL, input.substr(literal_start, literal_end - literal_start)});
                pos = literal_end + 1;
            }
            continue;
        }

        if (std::isdigit(input[pos])) {
            std::string::size_type num_start = pos;
            while (pos < input.length() && std::isdigit(input[pos])) {
                pos++;
            }
            tokens.push_back({TokenType::NUMBER, input.substr(num_start, pos - num_start)});
            continue;
        }

        // Identifiers & Keywords
        if (std::isalpha(input[pos])) {
            std::string::size_type ident_start = pos;
            while (pos < input.length() && (std::isalnum(input[pos]) || input[pos] == '-')) {
                pos++;
            }
            std::string value = input.substr(ident_start, pos - ident_start);
            auto it = keywords.find(value);
            if (it != keywords.end()) {
                 if (it->second == TokenType::KEYWORD_TEXT) {
                    tokens.push_back({it->second, value});
                    while (pos < input.length() && std::isspace(input[pos])) {
                        pos++;
                    }
                    if (pos < input.length() && input[pos] == '{') {
                        tokens.push_back({TokenType::L_BRACE, "{"});
                        pos++;

                        // Look for a quoted string first
                        while (pos < input.length() && std::isspace(input[pos])) {
                            pos++;
                        }

                        if (input[pos] == '"' || input[pos] == '\'') {
                            char quote_type = input[pos];
                            std::string::size_type literal_start = pos + 1;
                            std::string::size_type literal_end = input.find(quote_type, literal_start);
                            if (literal_end != std::string::npos) {
                                tokens.push_back({TokenType::STRING_LITERAL, input.substr(literal_start, literal_end - literal_start)});
                                pos = literal_end + 1;
                            } else {
                                // Unterminated string, treat as unknown
                                tokens.push_back({TokenType::UNKNOWN, input.substr(pos)});
                                pos = input.length();
                            }
                        } else {
                            // If no quoted string, treat as unquoted literal
                            std::string::size_type content_start = pos;
                            std::string::size_type content_end = input.find('}', content_start);

                            if (content_end != std::string::npos) {
                                // Trim leading spaces
                                while(content_start < content_end && std::isspace(input[content_start])) {
                                    content_start++;
                                }
                                // Trim trailing spaces
                                std::string::size_type temp_end = content_end;
                                while(temp_end > content_start && std::isspace(input[temp_end - 1])) {
                                    temp_end--;
                                }

                                std::string content = input.substr(content_start, temp_end - content_start);
                                if (!content.empty()) {
                                    tokens.push_back({TokenType::UNQUOTED_LITERAL, content});
                                }
                                pos = content_end;
                            }
                        }

                        while (pos < input.length() && std::isspace(input[pos])) {
                            pos++;
                        }
                        if (pos < input.length() && input[pos] == '}') {
                           tokens.push_back({TokenType::R_BRACE, "}"});
                           pos++;
                        }
                    }
                } else if (it->second == TokenType::KEYWORD_SCRIPT) {
                     tokens.push_back({TokenType::KEYWORD_SCRIPT, value});
                    while (pos < input.length() && std::isspace(input[pos])) {
                        pos++;
                    }
                    if (pos < input.length() && input[pos] == '{') {
                        tokens.push_back({TokenType::L_BRACE, "{"});
                        pos++;
                        int brace_level = 1;
                        std::string::size_type content_start = pos;
                        while (pos < input.length()) {
                            if (input[pos] == '{') brace_level++;
                            else if (input[pos] == '}') {
                                brace_level--;
                                if (brace_level == 0) break;
                            }
                            pos++;
                        }
                        if (brace_level == 0) {
                            tokens.push_back({TokenType::STRING_LITERAL, input.substr(content_start, pos - content_start)});
                            tokens.push_back({TokenType::R_BRACE, "}"});
                            pos++;
                        }
                    }
                } else {
                    tokens.push_back({it->second, value});
                }
            } else if (value == "at") {
                std::string::size_type next_word_pos = pos;
                while (next_word_pos < input.length() && std::isspace(input[next_word_pos])) {
                    next_word_pos++;
                }
                if (input.substr(next_word_pos, 3) == "top") {
                    tokens.push_back({TokenType::KEYWORD_AT_TOP, "at top"});
                    pos = next_word_pos + 3;
                } else if (input.substr(next_word_pos, 6) == "bottom") {
                    tokens.push_back({TokenType::KEYWORD_AT_BOTTOM, "at bottom"});
                    pos = next_word_pos + 6;
                } else {
                    tokens.push_back({TokenType::IDENTIFIER, value});
                }
            }
            else { tokens.push_back({TokenType::IDENTIFIER, value}); }
            continue;
        }

        tokens.push_back({TokenType::UNKNOWN, std::string(1, input[pos])});
        pos++;
    }

    return tokens;
}

} // namespace CHTL