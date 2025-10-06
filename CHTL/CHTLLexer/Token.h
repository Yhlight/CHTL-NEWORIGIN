#pragma once

#include <string>
#include <vector>

namespace CHTL {

    enum class TokenType {
        // Literals
        TOKEN_IDENTIFIER,
        TOKEN_STRING_LITERAL,
        TOKEN_NUMERIC_LITERAL,
        TOKEN_UNQUOTED_LITERAL,

        // Keywords
        TOKEN_TEXT,
        TOKEN_STYLE,
        TOKEN_SCRIPT,
        TOKEN_IF,
        TOKEN_ELSE,
        TOKEN_ELSE_IF,
        TOKEN_EXCEPT,
        TOKEN_DELETE,
        TOKEN_INSERT,
        TOKEN_KEYWORD_USE,
        TOKEN_KEYWORD_HTML5,
        TOKEN_KEYWORD_FROM,

        // Punctuation
        TOKEN_AT,
        TOKEN_LBRACE,
        TOKEN_RBRACE,
        TOKEN_LBRACE_DOUBLE,
        TOKEN_RBRACE_DOUBLE,
        TOKEN_LPAREN,
        TOKEN_RPAREN,
        TOKEN_LBRACKET,
        TOKEN_RBRACKET,
        TOKEN_COLON,
        TOKEN_SEMICOLON,
        TOKEN_COMMA,
        TOKEN_DOT,
        TOKEN_HASH,

        // Operators
        TOKEN_ASSIGN,
        TOKEN_PLUS,
        TOKEN_MINUS,
        TOKEN_MULTIPLY,
        TOKEN_DIVIDE,
        TOKEN_MODULO,
        TOKEN_POWER,
        TOKEN_GT,
        TOKEN_LT,
        TOKEN_GTE,
        TOKEN_LTE,
        TOKEN_EQ,
        TOKEN_NEQ,
        TOKEN_AND,
        TOKEN_OR,
        TOKEN_QUESTION,

        // Comments
        TOKEN_SINGLE_LINE_COMMENT,
        TOKEN_MULTI_LINE_COMMENT,
        TOKEN_GENERATOR_COMMENT,

        // Special Tokens
        TOKEN_EOF,
        TOKEN_UNKNOWN
    };

    struct Token {
        TokenType type;
        std::string lexeme;
        int line;
        int column;
    };

}