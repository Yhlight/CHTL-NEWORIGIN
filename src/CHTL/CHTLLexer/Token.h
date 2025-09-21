#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <map>

namespace CHTL {

    enum TokenType {
        // Special Tokens
        ILLEGAL, // A token/character we don't know about
        END_OF_FILE,

        // Identifiers + Literals
        IDENTIFIER, // add, foobar, x, y, ...
        STRING,     // "foobar"

        // Operators & Delimiters
        COLON,      // :
        SEMICOLON,  // ;
        LBRACE,     // {
        RBRACE,     // }

        // Keywords will be added later
    };

    const std::map<TokenType, std::string> TokenTypeStrings = {
        {ILLEGAL, "ILLEGAL"},
        {END_OF_FILE, "EOF"},
        {IDENTIFIER, "IDENTIFIER"},
        {STRING, "STRING"},
        {COLON, "COLON"},
        {SEMICOLON, "SEMICOLON"},
        {LBRACE, "LBRACE"},
        {RBRACE, "RBRACE"},
    };

    struct Token {
        TokenType type;
        std::string literal;
        int line;
        int column;

        std::string ToString() const {
            return "Line " + std::to_string(line) + ":" + std::to_string(column) +
                   " - Type: " + TokenTypeStrings.at(type) + ", Literal: '" + literal + "'";
        }
    };

} // namespace CHTL

#endif // TOKEN_H
