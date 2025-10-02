/**
 * @file Lexer.h  
 * @brief CHTL Lexical Analyzer
 */

#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "Token.h"
#include <vector>
#include <string>

namespace CHTL {

class Lexer {
public:
    explicit Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    std::string source_;
    size_t current_;
    int line_;
    int column_;
    
    Token scanToken();
    char advance();
    char peek() const;
    char peekNext() const;
    bool isAtEnd() const;
    bool match(char expected);
    
    Token identifier();
    Token number();
    Token string(char quote);
    Token unquotedLiteral();
};

} // namespace CHTL

#endif
