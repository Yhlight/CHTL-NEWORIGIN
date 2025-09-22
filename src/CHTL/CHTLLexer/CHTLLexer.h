#pragma once

#include <string>
#include "Token.h"

namespace CHTL {

    class CHTLLexer {
    public:
        CHTLLexer(const std::string& source);
        Token getNextToken();

    private:
        std::string source;
        size_t position;
        int line;
        int column;

        void advance();
        char currentChar();
        char peek();
        void skipWhitespace();
        Token identifier();
        Token stringLiteral(char quoteType);
        Token numberLiteral();

        void skipSingleLineComment();
        void skipMultiLineComment();
        Token generatorComment();
    };

} // namespace CHTL
