#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "Token.h"
#include <string>
#include <vector>

namespace CHTL {

class Lexer {
public:
    explicit Lexer(const std::string& source);
    std::vector<Token> Tokenize();

private:
    Token GetNextToken();
    char Peek();
    char Advance();
    void SkipWhitespace();
    void SkipSingleLineComment();
    void SkipMultiLineComment();
    Token MakeIdentifier();
    Token MakeString();

    const std::string& source;
    size_t current = 0;
    // int line = 1; // For future error reporting
};

} // namespace CHTL

#endif // CHTL_LEXER_H
