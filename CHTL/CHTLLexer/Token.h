#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>

namespace CHTL {

enum class TokenType {
    // Basic
    UNKNOWN,
    EndOfFile,
    IDENTIFIER,
    STRING_LITERAL,
    UNQUOTED_LITERAL,
    NUMBER,
    COMMENT,

    // Keywords
    KEYWORD_TEXT,
    KEYWORD_STYLE,
    KEYWORD_SCRIPT,
    KEYWORD_INHERIT,
    KEYWORD_DELETE,
    KEYWORD_INSERT,
    KEYWORD_AFTER,
    KEYWORD_BEFORE,
    KEYWORD_REPLACE,
    KEYWORD_AT_TOP,
    KEYWORD_AT_BOTTOM,
    KEYWORD_FROM,
    KEYWORD_AS,
    KEYWORD_EXCEPT,
    KEYWORD_USE,
    KEYWORD_HTML5,

    // Block Keywords
    KEYWORD_CUSTOM,
    KEYWORD_TEMPLATE,
    KEYWORD_ORIGIN,
    KEYWORD_IMPORT,
    KEYWORD_NAMESPACE,
    KEYWORD_CONFIGURATION,
    KEYWORD_INFO,
    KEYWORD_EXPORT,
    KEYWORD_ORIGIN_TYPE,
    KEYWORD_NAME,

    // Type Keywords
    TYPE_STYLE,
    TYPE_ELEMENT,
    TYPE_VAR,
    TYPE_HTML,
    TYPE_JAVASCRIPT,
    TYPE_CHTL,
    TYPE_CJMOD,
    TYPE_CONFIG,


    // Punctuation
    L_BRACE, // {
    R_BRACE, // }
    L_PAREN, // (
    R_PAREN, // )
    L_BRACKET, // [
    R_BRACKET, // ]
    COLON, // :
    EQUAL, // =
    SEMICOLON, // ;
    COMMA, // ,
    DOT, // .
    AMPERSAND, // &
    QUESTION, // ?
    DOLLAR, // $
    AT, // @
    HASH, // #

    // CHTL JS
    DOUBLE_L_BRACE, // {{
    DOUBLE_R_BRACE, // }}
    ARROW, // ->
    EVENT_BIND_OP, // &->

    // Operators
    PLUS, // +
    MINUS, // -
    STAR, // *
    SLASH, // /
    PERCENT, // %
    STAR_STAR, // **

    // Logical Operators
    DOUBLE_AMPERSAND, // &&
    DOUBLE_PIPE, // ||
    EXCLAMATION, // !

    // Comparison Operators
    DOUBLE_EQUAL, // ==
    NOT_EQUAL, // !=
    LESS, // <
    GREATER, // >
    LESS_EQUAL, // <=
    GREATER_EQUAL, // >=

    // Assignment Operators
    PLUS_EQUAL, // +=
    MINUS_EQUAL, // -=
    STAR_EQUAL, // *=
    SLASH_EQUAL, // /=
};

class Token {
public:
    TokenType type;
    std::string value;

    // Helper for debugging
    bool operator==(const Token& other) const {
        return type == other.type && value == other.value;
    }
};

} // namespace CHTL

#endif // CHTL_TOKEN_H