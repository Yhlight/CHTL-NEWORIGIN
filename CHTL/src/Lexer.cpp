#include "Lexer.hpp"

namespace chtl::core {

Lexer::Lexer(const std::string& source) : src(source) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> out;
    int line = 1, col = 1;
    size_t i = 0;
    auto push = [&](TokenType t, const std::string& lx){ out.push_back({t, lx, line, col}); };
    while (i < src.size()) {
        char c = src[i];
        if (c == '\n') { ++line; col = 1; ++i; continue; }
        if (std::isspace(static_cast<unsigned char>(c))) { ++col; ++i; continue; }
        if (c == '/' && i + 1 < src.size()) {
            if (src[i+1] == '/') { // line comment
                i += 2; col += 2;
                while (i < src.size() && src[i] != '\n') { ++i; ++col; }
                continue;
            } else if (src[i+1] == '*') { // block comment
                i += 2; col += 2;
                while (i + 1 < src.size() && !(src[i] == '*' && src[i+1] == '/')) {
                    if (src[i] == '\n') { ++line; col = 1; } else { ++col; }
                    ++i;
                }
                if (i + 1 < src.size()) { i += 2; col += 2; }
                push(TokenType::Comment, "/* */");
                continue;
            }
        }
        if (c == '"' || c == '\'') {
            char quote = c; size_t j = i + 1; int col0 = col;
            while (j < src.size()) {
                char d = src[j];
                if (d == '\n') { ++line; col = 1; }
                if (d == quote && src[j-1] != '\\') { ++j; break; }
                ++j; ++col;
            }
            push(TokenType::String, std::string(src.substr(i, j - i)));
            col += static_cast<int>(j - i); i = j; continue;
        }
        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_' || c == '@' || c == '[' || c == ']') {
            size_t j = i + 1;
            while (j < src.size() && (std::isalnum(static_cast<unsigned char>(src[j])) || src[j]=='_' || src[j]=='-' || src[j]==':' || src[j]=='@' || src[j]=='[' || src[j]==']')) ++j;
            push(TokenType::Identifier, std::string(src.substr(i, j - i)));
            col += static_cast<int>(j - i); i = j; continue;
        }
        if (std::isdigit(static_cast<unsigned char>(c))) {
            size_t j = i + 1;
            while (j < src.size() && (std::isdigit(static_cast<unsigned char>(src[j])) || src[j]=='.')) ++j;
            push(TokenType::Number, std::string(src.substr(i, j - i)));
            col += static_cast<int>(j - i); i = j; continue;
        }
        if (c == '{') { push(TokenType::LBrace, "{"); ++i; ++col; continue; }
        if (c == '}') { push(TokenType::RBrace, "}"); ++i; ++col; continue; }
        if (c == ':') { push(TokenType::Colon, ":"); ++i; ++col; continue; }
        if (c == '=') { push(TokenType::Equal, "="); ++i; ++col; continue; }
        if (c == ';') { push(TokenType::Semicolon, ";"); ++i; ++col; continue; }
        // fallback: consume single char as identifier-like
        push(TokenType::Identifier, std::string(1, c)); ++i; ++col;
    }
    out.push_back({ TokenType::EndOfFile, "", line, col });
    return out;
}

}
