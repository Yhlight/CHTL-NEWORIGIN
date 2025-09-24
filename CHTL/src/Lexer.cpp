#include "Lexer.hpp"

namespace chtl::core {

Lexer::Lexer(const std::string& source) : src(source) {}

std::vector<Token> Lexer::tokenize() {
    // Minimal stub: single EOF token.
    return { Token{ TokenType::EndOfFile, "", 0, 0 } };
}

}
