#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "CHTL/CHTLLexer/Token.h"
#include <string>
#include <vector>

namespace CHTL {

class CHTLLexer {
public:
    CHTLLexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    std::string source;
    int current_pos;
    int line;
    int column;

    Token getNextToken();
    char peek();
    char advance();
    void skipWhitespace();
    void skipComment();
    // Helper functions for tokenizing different types
    Token makeIdentifier();
    Token makeString();
    Token makeUnquotedLiteral();
    Token makeNumber();
};

} // namespace CHTL

#endif // CHTL_LEXER_H
