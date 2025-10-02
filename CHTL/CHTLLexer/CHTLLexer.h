#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "Token.h"
#include <string>
#include <vector>

class CHTLLexer {
public:
    explicit CHTLLexer(const std::string& input);
    Token getNextToken();
    std::vector<Token> getAllTokens();

private:
    std::string input;
    size_t position;
    size_t line;
    size_t column;

    char currentChar();
    void advance();
    void skipWhitespace();
    void skipSingleLineComment();
    void skipMultiLineComment();
    Token makeIdentifier();
    Token makeString(char quote);
    Token makeComment();
    Token makeToken(TokenType type, const std::string& value);
};

#endif //CHTL_LEXER_H