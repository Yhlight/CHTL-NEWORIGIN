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
        if (input[pos] == '#' && pos + 1 < input.length() && input[pos + 1] == ' ') {
            std::string::size_type comment_start = pos + 2;
            std::string::size_type comment_end = input.find('\n', comment_start);
            tokens.push_back({TokenType::COMMENT, (comment_end == std::string::npos) ? input.substr(comment_start) : input.substr(comment_start, comment_end - comment_start)});
            pos = (comment_end == std::string::npos) ? input.length() : comment_end;
            continue;
        }

        // --- Operators and Punctuation ---
        if (input[pos] == '+') { tokens.push_back({TokenType::PLUS, "+"}); pos++; continue; }
        if (input[pos] == '-') { tokens.push_back({TokenType::MINUS, "-"}); pos++; continue; }
        if (input[pos] == '*') {
            if (pos + 1 < input.length() && input[pos + 1] == '*') {
                tokens.push_back({TokenType::STAR_STAR, "**"}); pos += 2;
            } else {
                tokens.push_back({TokenType::STAR, "*"}); pos++;
            }
            continue;
        }
        if (input[pos] == '/') { tokens.push_back({TokenType::SLASH, "/"}); pos++; continue; }
        if (input[pos] == '%') { tokens.push_back({TokenType::PERCENT, "%"}); pos++; continue; }
        if (input[pos] == '{') { tokens.push_back({TokenType::L_BRACE, "{"}); pos++; continue; }
        if (input[pos] == '}') { tokens.push_back({TokenType::R_BRACE, "}"}); pos++; continue; }
        if (input[pos] == ':') { tokens.push_back({TokenType::COLON, ":"}); pos++; continue; }
        if (input[pos] == ';') { tokens.push_back({TokenType::SEMICOLON, ";"}); pos++; continue; }
        if (input[pos] == '?') { tokens.push_back({TokenType::QUESTION_MARK, "?"}); pos++; continue; }
        if (input[pos] == '.') { tokens.push_back({TokenType::DOT, "."}); pos++; continue; }
        if (input[pos] == '#') { tokens.push_back({TokenType::HASH, "#"}); pos++; continue; }
        if (input[pos] == '[') { tokens.push_back({TokenType::L_BRACKET, "["}); pos++; continue; }
        if (input[pos] == ']') { tokens.push_back({TokenType::R_BRACKET, "]"}); pos++; continue; }
        if (input[pos] == '@') { tokens.push_back({TokenType::AT_SIGN, "@"}); pos++; continue; }
        if (input[pos] == ',') { tokens.push_back({TokenType::COMMA, ","}); pos++; continue; }

        if (input[pos] == '>') {
            if (pos + 1 < input.length() && input[pos + 1] == '=') {
                tokens.push_back({TokenType::GREATER_EQUAL, ">="}); pos += 2;
            } else {
                tokens.push_back({TokenType::GREATER, ">"}); pos++;
            }
            continue;
        }
        if (input[pos] == '<') {
            if (pos + 1 < input.length() && input[pos + 1] == '=') {
                tokens.push_back({TokenType::LESS_EQUAL, "<="}); pos += 2;
            } else {
                tokens.push_back({TokenType::LESS, "<"}); pos++;
            }
            continue;
        }
        if (input[pos] == '=') {
            if (pos + 1 < input.length() && input[pos + 1] == '=') {
                tokens.push_back({TokenType::EQUAL_EQUAL, "=="}); pos += 2;
            } else {
                tokens.push_back({TokenType::EQUAL, "="}); pos++;
            }
            continue;
        }
        if (input[pos] == '!') {
            if (pos + 1 < input.length() && input[pos + 1] == '=') {
                tokens.push_back({TokenType::BANG_EQUAL, "!="}); pos += 2;
            } else {
                tokens.push_back({TokenType::UNKNOWN, "!"}); pos++;
            }
            continue;
        }
        if (input[pos] == '&') {
            if (pos + 1 < input.length() && input[pos + 1] == '&') {
                tokens.push_back({TokenType::AMPERSAND_AMPERSAND, "&&"}); pos += 2;
            } else {
                tokens.push_back({TokenType::AMPERSAND, "&"}); pos++;
            }
            continue;
        }
        if (input[pos] == '|') {
            if (pos + 1 < input.length() && input[pos + 1] == '|') {
                tokens.push_back({TokenType::PIPE_PIPE, "||"}); pos += 2;
            } else {
                tokens.push_back({TokenType::UNKNOWN, "|"}); pos++;
            }
            continue;
        }

        if (input[pos] == '"' || input[pos] == '\'') {
            char quote_char = input[pos];
            std::string::size_type literal_start = pos + 1;
            std::string::size_type literal_end = input.find(quote_char, literal_start);
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

        if (std::isalpha(input[pos])) {
            std::string::size_type ident_start = pos;
            while (pos < input.length() && (std::isalnum(input[pos]) || input[pos] == '-')) {
                pos++;
            }
            std::string value = input.substr(ident_start, pos - ident_start);

            if (value == "text") { tokens.push_back({TokenType::TEXT_KEYWORD, value}); }
            else if (value == "style") { tokens.push_back({TokenType::STYLE_KEYWORD, value}); }
            else if (value == "script") {
                tokens.push_back({TokenType::SCRIPT_KEYWORD, value});
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