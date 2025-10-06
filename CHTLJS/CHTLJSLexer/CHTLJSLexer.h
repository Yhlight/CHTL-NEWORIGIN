#pragma once

#include "Token.h"
#include <string>
#include <vector>

namespace CHTLJS {

    class CHTLJSLexer {
    public:
        CHTLJSLexer(const std::string& source);
        Token getNextToken();

    private:
        std::string source;
        size_t position;
        int line;
        int column;

        char peek();
        char advance();
        void skipWhitespace();
        Token makeToken(TokenType type, const std::string& lexeme);
        Token lexIdentifier();
        Token stringLiteral();
        Token errorToken(const std::string& message);
    };

}