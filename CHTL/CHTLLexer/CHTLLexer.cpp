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

        // --- Arithmetic Operators ---
        if (input[pos] == '+') { tokens.push_back({TokenType::PLUS, "+"}); pos++; continue; }
        if (input[pos] == '-') { tokens.push_back({TokenType::MINUS, "-"}); pos++; continue; }
        if (input[pos] == '*') {
            if (pos + 1 < input.length() && input[pos + 1] == '*') {
                tokens.push_back({TokenType::STAR_STAR, "**"});
                pos += 2;
            } else {
                tokens.push_back({TokenType::STAR, "*"});
                pos++;
            }
            continue;
        }
        if (input[pos] == '/') { tokens.push_back({TokenType::SLASH, "/"}); pos++; continue; }
        if (input[pos] == '%') { tokens.push_back({TokenType::PERCENT, "%"}); pos++; continue; }


        if (input[pos] == '{') { tokens.push_back({TokenType::L_BRACE, "{"}); pos++; continue; }
        if (input[pos] == '}') { tokens.push_back({TokenType::R_BRACE, "}"}); pos++; continue; }
        if (input[pos] == '[') { tokens.push_back({TokenType::L_BRACKET, "["}); pos++; continue; }
        if (input[pos] == ']') { tokens.push_back({TokenType::R_BRACKET, "]"}); pos++; continue; }
        if (input[pos] == ':') { tokens.push_back({TokenType::COLON, ":"}); pos++; continue; }
        if (input[pos] == '=') { tokens.push_back({TokenType::EQUAL, "="}); pos++; continue; }
        if (input[pos] == ';') { tokens.push_back({TokenType::SEMICOLON, ";"}); pos++; continue; }
        if (input[pos] == '@') { tokens.push_back({TokenType::AT_SIGN, "@"}); pos++; continue; }

        if (input[pos] == '"') {
            std::string::size_type literal_start = pos + 1;
            std::string::size_type literal_end = input.find('"', literal_start);
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
            TokenType type = TokenType::IDENTIFIER;

            if (value == "Template") { type = TokenType::KEYWORD_TEMPLATE; }
            else if (value == "Style") { type = TokenType::KEYWORD_STYLE; }
            else if (value == "Element") { type = TokenType::KEYWORD_ELEMENT; }
            else if (value == "Var") { type = TokenType::KEYWORD_VAR; }
            else if (value == "text") { type = TokenType::KEYWORD_TEXT; }
            else if (value == "style") { type = TokenType::KEYWORD_STYLE; }
            else if (value == "script") { type = TokenType::KEYWORD_SCRIPT; }
            else if (value == "Custom") { type = TokenType::KEYWORD_CUSTOM; }
            else if (value == "Origin") { type = TokenType::KEYWORD_ORIGIN; }
            else if (value == "Import") { type = TokenType::KEYWORD_IMPORT; }
            else if (value == "Namespace") { type = TokenType::KEYWORD_NAMESPACE; }
            else if (value == "use") { type = TokenType::KEYWORD_USE; }
            else if (value == "from") { type = TokenType::KEYWORD_FROM; }
            else if (value == "as") { type = TokenType::KEYWORD_AS; }
            else if (value == "delete") { type = TokenType::KEYWORD_DELETE; }
            else if (value == "insert") { type = TokenType::KEYWORD_INSERT; }
            else if (value == "inherit") { type = TokenType::KEYWORD_INHERIT; }

            tokens.push_back({type, value});
            continue;
        }

        tokens.push_back({TokenType::UNKNOWN, std::string(1, input[pos])});
        pos++;
    }

    return tokens;
}

} // namespace CHTL