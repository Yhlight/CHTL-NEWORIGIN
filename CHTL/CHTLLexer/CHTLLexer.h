#pragma once

#include "Token.h"
#include <string>
#include <vector>

namespace CHTL {

class CHTLLexer {
public:
    explicit CHTLLexer(const std::string& source);

    // Returns the next token from the source code.
    Token getNextToken();

    // Helper to get all tokens at once.
    std::vector<Token> getAllTokens();

private:
    // Advances the current position and handles line/column counts.
    void advance();
    char currentChar() const;

    // Skips over non-token characters.
    void skipWhitespace();
    void skipLineComment();
    void skipBlockComment();

    // Token recognition methods.
    Token identifier();
    Token number();
    Token stringLiteral();
    Token makeToken(TokenType type, const std::string& value);
    Token makeToken(TokenType type, char value);


    std::string m_source;
    size_t m_position = 0;
    int m_line = 1;
    int m_column = 1;
};

} // namespace CHTL
