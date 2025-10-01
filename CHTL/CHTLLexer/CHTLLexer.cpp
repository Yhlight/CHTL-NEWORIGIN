#include "CHTLLexer.h"
#include <cctype>

namespace CHTL {

CHTLLexer::CHTLLexer() {}

CHTLLexer::~CHTLLexer() {}

std::vector<Token> CHTLLexer::tokenize(const std::string& input) {
    std::vector<Token> tokens;
    std::string::size_type pos = 0;

    while (pos < input.length()) {
        if (std::isspace(input[pos])) {
            pos++;
            continue;
        }

        if (pos + 1 < input.length() && input[pos] == '/') {
            if (input[pos + 1] == '/') {
                std::string::size_type comment_start = pos + 2;
                std::string::size_type comment_end = input.find('\n', comment_start);
                tokens.push_back({TokenType::COMMENT, (comment_end == std::string::npos) ? input.substr(comment_start) : input.substr(comment_start, comment_end - comment_start), pos});
                pos = (comment_end == std::string::npos) ? input.length() : comment_end;
                continue;
            }
            if (input[pos + 1] == '*') {
                std::string::size_type comment_start = pos + 2;
                std::string::size_type comment_end = input.find("*/", comment_start);
                if (comment_end == std::string::npos) {
                    tokens.push_back({TokenType::COMMENT, input.substr(comment_start), pos});
                    pos = input.length();
                } else {
                    tokens.push_back({TokenType::COMMENT, input.substr(comment_start, comment_end - comment_start), pos});
                    pos = comment_end + 2;
                }
                continue;
            }
        }
        if (input[pos] == '#' && pos + 1 < input.length() && input[pos + 1] == ' ') {
            std::string::size_type comment_start = pos + 2;
            std::string::size_type comment_end = input.find('\n', comment_start);
            tokens.push_back({TokenType::COMMENT, (comment_end == std::string::npos) ? input.substr(comment_start) : input.substr(comment_start, comment_end - comment_start), pos});
            pos = (comment_end == std::string::npos) ? input.length() : comment_end;
            continue;
        }

        // --- Operators and Punctuation ---
        if (input[pos] == '+') { tokens.push_back({TokenType::PLUS, "+", pos}); pos++; continue; }
        if (input[pos] == '-') { tokens.push_back({TokenType::MINUS, "-", pos}); pos++; continue; }
        if (input[pos] == '*') {
            if (pos + 1 < input.length() && input[pos + 1] == '*') {
                tokens.push_back({TokenType::STAR_STAR, "**", pos}); pos += 2;
            } else {
                tokens.push_back({TokenType::STAR, "*", pos}); pos++;
            }
            continue;
        }
        if (input[pos] == '/') { tokens.push_back({TokenType::SLASH, "/", pos}); pos++; continue; }
        if (input[pos] == '%') { tokens.push_back({TokenType::PERCENT, "%", pos}); pos++; continue; }
        if (input[pos] == '{') { tokens.push_back({TokenType::L_BRACE, "{", pos}); pos++; continue; }
        if (input[pos] == '}') { tokens.push_back({TokenType::R_BRACE, "}", pos}); pos++; continue; }
        if (input[pos] == ':') { tokens.push_back({TokenType::COLON, ":", pos}); pos++; continue; }
        if (input[pos] == ';') { tokens.push_back({TokenType::SEMICOLON, ";", pos}); pos++; continue; }
        if (input[pos] == '?') { tokens.push_back({TokenType::QUESTION_MARK, "?", pos}); pos++; continue; }
        if (input[pos] == '.') { tokens.push_back({TokenType::DOT, ".", pos}); pos++; continue; }
        if (input[pos] == '#') {
            size_t start_pos = pos + 1;
            size_t end_pos = start_pos;
            while (end_pos < input.length() && std::isxdigit(input[end_pos])) {
                end_pos++;
            }
            size_t len = end_pos - start_pos;
            if (len == 3 || len == 6 || len == 8) {
                tokens.push_back({TokenType::HEX_LITERAL, input.substr(pos, len + 1), pos});
                pos = end_pos;
            } else {
                tokens.push_back({TokenType::HASH, "#", pos});
                pos++;
            }
            continue;
        }
        if (input[pos] == '[') { tokens.push_back({TokenType::L_BRACKET, "[", pos}); pos++; continue; }
        if (input[pos] == ']') { tokens.push_back({TokenType::R_BRACKET, "]", pos}); pos++; continue; }
        if (input[pos] == '@') { tokens.push_back({TokenType::AT_SIGN, "@", pos}); pos++; continue; }
        if (input[pos] == '(') { tokens.push_back({TokenType::L_PAREN, "(", pos}); pos++; continue; }
        if (input[pos] == ')') { tokens.push_back({TokenType::R_PAREN, ")", pos}); pos++; continue; }
        if (input[pos] == ',') { tokens.push_back({TokenType::COMMA, ",", pos}); pos++; continue; }

        if (input[pos] == '>') {
            if (pos + 1 < input.length() && input[pos + 1] == '=') {
                tokens.push_back({TokenType::GREATER_EQUAL, ">=", pos}); pos += 2;
            } else {
                tokens.push_back({TokenType::GREATER, ">", pos}); pos++;
            }
            continue;
        }
        if (input[pos] == '<') {
            if (pos + 1 < input.length() && input[pos + 1] == '=') {
                tokens.push_back({TokenType::LESS_EQUAL, "<=", pos}); pos += 2;
            } else {
                tokens.push_back({TokenType::LESS, "<", pos}); pos++;
            }
            continue;
        }
        if (input[pos] == '=') {
            if (pos + 1 < input.length() && input[pos + 1] == '=') {
                tokens.push_back({TokenType::EQUAL_EQUAL, "==", pos}); pos += 2;
            } else {
                tokens.push_back({TokenType::EQUAL, "=", pos}); pos++;
            }
            continue;
        }
        if (input[pos] == '!') {
            if (pos + 1 < input.length() && input[pos + 1] == '=') {
                tokens.push_back({TokenType::BANG_EQUAL, "!=", pos}); pos += 2;
            } else {
                tokens.push_back({TokenType::UNKNOWN, "!", pos}); pos++;
            }
            continue;
        }
        if (input[pos] == '&') {
            if (pos + 1 < input.length() && input[pos + 1] == '&') {
                tokens.push_back({TokenType::AMPERSAND_AMPERSAND, "&&", pos}); pos += 2;
            } else {
                tokens.push_back({TokenType::AMPERSAND, "&", pos}); pos++;
            }
            continue;
        }
        if (input[pos] == '|') {
            if (pos + 1 < input.length() && input[pos + 1] == '|') {
                tokens.push_back({TokenType::PIPE_PIPE, "||", pos}); pos += 2;
            } else {
                tokens.push_back({TokenType::UNKNOWN, "|", pos}); pos++;
            }
            continue;
        }

        if (input[pos] == '"' || input[pos] == '\'') {
            char quote_char = input[pos];
            std::string::size_type literal_start = pos + 1;
            std::string::size_type literal_end = input.find(quote_char, literal_start);
            if (literal_end == std::string::npos) {
                tokens.push_back({TokenType::UNKNOWN, input.substr(pos), pos});
                pos = input.length();
            } else {
                tokens.push_back({TokenType::STRING_LITERAL, input.substr(literal_start, literal_end - literal_start), pos});
                pos = literal_end + 1;
            }
            continue;
        }

        if (std::isdigit(input[pos])) {
            std::string::size_type num_start = pos;
            while (pos < input.length() && std::isdigit(input[pos])) {
                pos++;
            }
            tokens.push_back({TokenType::NUMBER, input.substr(num_start, pos - num_start), num_start});
            continue;
        }

        if (std::isalpha(input[pos])) {
            std::string::size_type ident_start = pos;
            while (pos < input.length() && (std::isalnum(input[pos]) || input[pos] == '-')) {
                pos++;
            }
            std::string value = input.substr(ident_start, pos - ident_start);

            if (value == "text") { tokens.push_back({TokenType::TEXT_KEYWORD, value, ident_start}); }
            else if (value == "style") { tokens.push_back({TokenType::STYLE_KEYWORD, value, ident_start}); }
            else if (value == "use") { tokens.push_back({TokenType::USE_KEYWORD, value, ident_start}); }
            else if (value == "html5") { tokens.push_back({TokenType::HTML5_KEYWORD, value, ident_start}); }
            else if (value == "inherit") { tokens.push_back({TokenType::INHERIT_KEYWORD, value, ident_start}); }
            else if (value == "Custom") { tokens.push_back({TokenType::CUSTOM_KEYWORD, value, ident_start}); }
            else if (value == "Origin") { tokens.push_back({TokenType::ORIGIN_KEYWORD, value, ident_start}); }
            else if (value == "delete") { tokens.push_back({TokenType::DELETE_KEYWORD, value, ident_start}); }
            else if (value == "insert") { tokens.push_back({TokenType::INSERT_KEYWORD, value, ident_start}); }
            else if (value == "after") { tokens.push_back({TokenType::AFTER_KEYWORD, value, ident_start}); }
            else if (value == "before") { tokens.push_back({TokenType::BEFORE_KEYWORD, value, ident_start}); }
            else if (value == "replace") { tokens.push_back({TokenType::REPLACE_KEYWORD, value, ident_start}); }
            else if (value == "at") { tokens.push_back({TokenType::AT_KEYWORD, value, ident_start}); }
            else if (value == "top") { tokens.push_back({TokenType::TOP_KEYWORD, value, ident_start}); }
            else if (value == "bottom") { tokens.push_back({TokenType::BOTTOM_KEYWORD, value, ident_start}); }
            else if (value == "if") { tokens.push_back({TokenType::IF_KEYWORD, value, ident_start}); }
            else if (value == "else") { tokens.push_back({TokenType::ELSE_KEYWORD, value, ident_start}); }
            else if (value == "condition") { tokens.push_back({TokenType::CONDITION_KEYWORD, value, ident_start}); }
            else if (value == "script") {
                tokens.push_back({TokenType::SCRIPT_KEYWORD, value, ident_start});
                while (pos < input.length() && std::isspace(input[pos])) {
                    pos++;
                }
                if (pos < input.length() && input[pos] == '{') {
                    tokens.push_back({TokenType::L_BRACE, "{", pos});
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
                        tokens.push_back({TokenType::STRING_LITERAL, input.substr(content_start, pos - content_start), content_start});
                        tokens.push_back({TokenType::R_BRACE, "}", pos});
                        pos++;
                    }
                }
            }
            else { tokens.push_back({TokenType::IDENTIFIER, value, ident_start}); }
            continue;
        }

        tokens.push_back({TokenType::UNKNOWN, std::string(1, input[pos]), pos});
        pos++;
    }

    return tokens;
}

} // namespace CHTL