#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "Token.h"
#include <string>
#include <vector>

namespace CHTL
{
    class Lexer
    {
    public:
        Lexer(const std::string& source);

        std::vector<Token> Tokenize();

    private:
        Token GetNextToken();
        char Peek();
        char Advance();
        bool IsAtEnd();

        void SkipWhitespaceAndComments();
        Token HandleIdentifier();
        Token HandleStringLiteral(char quoteType);
        Token HandleNumberLiteral();
        Token HandleSymbol();

        const std::string& m_source;
        size_t m_current = 0;
        size_t m_line = 1;
        size_t m_column = 1;
    };
}

#endif // CHTL_LEXER_H
