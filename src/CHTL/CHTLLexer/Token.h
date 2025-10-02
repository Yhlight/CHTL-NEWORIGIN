/**
 * @file Token.h
 * @brief CHTL Token Definition
 * 
 * Defines token types and token structure for lexical analysis
 */

#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <vector>

namespace CHTL {

/**
 * @enum TokenType
 * @brief Types of tokens in CHTL
 */
enum class TokenType {
    // Basic tokens
    IDENTIFIER,         // element names, variable names
    STRING_LITERAL,     // "text", 'text'
    UNQUOTED_LITERAL,   // unquoted text
    NUMBER,             // 123, 45.6
    
    // Operators
    COLON,              // :
    SEMICOLON,          // ;
    COMMA,              // ,
    DOT,                // .
    EQUAL,              // =
    PLUS,               // +
    MINUS,              // -
    STAR,               // *
    SLASH,              // /
    PERCENT,            // %
    DOUBLE_STAR,        // **
    
    // Comparison operators
    LESS,               // <
    GREATER,            // >
    LESS_EQUAL,         // <=
    GREATER_EQUAL,      // >=
    EQUAL_EQUAL,        // ==
    NOT_EQUAL,          // !=
    
    // Logical operators
    AND,                // &&
    OR,                 // ||
    
    // Delimiters
    LEFT_BRACE,         // {
    RIGHT_BRACE,        // }
    LEFT_BRACKET,       // [
    RIGHT_BRACKET,      // ]
    LEFT_PAREN,         // (
    RIGHT_PAREN,        // )
    
    // Keywords
    KEYWORD_TEXT,       // text
    KEYWORD_STYLE,      // style
    KEYWORD_SCRIPT,     // script
    KEYWORD_IF,         // if
    KEYWORD_ELSE,       // else
    KEYWORD_TEMPLATE,   // [Template]
    KEYWORD_CUSTOM,     // [Custom]
    KEYWORD_ORIGIN,     // [Origin]
    KEYWORD_IMPORT,     // [Import]
    KEYWORD_NAMESPACE,  // [Namespace]
    KEYWORD_CONFIG,     // [Configuration]
    KEYWORD_DELETE,     // delete
    KEYWORD_INSERT,     // insert
    KEYWORD_INHERIT,    // inherit
    KEYWORD_FROM,       // from
    KEYWORD_AS,         // as
    KEYWORD_USE,        // use
    KEYWORD_EXCEPT,     // except
    
    // Special tokens
    AT_SYMBOL,          // @
    AMPERSAND,          // &
    ARROW,              // ->
    QUESTION,           // ?
    
    // Comments
    LINE_COMMENT,       // //
    BLOCK_COMMENT,      // /* */
    GENERATOR_COMMENT,  // # 
    
    // End of file
    EOF_TOKEN,
    
    // Error token
    ERROR
};

/**
 * @struct Token
 * @brief Represents a single token
 */
struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
    
    Token(TokenType t = TokenType::ERROR, 
          const std::string& lex = "", 
          int l = 0, 
          int c = 0)
        : type(t), lexeme(lex), line(l), column(c) {}
    
    std::string toString() const;
};

/**
 * @brief Check if token is a keyword
 */
bool isKeyword(TokenType type);

/**
 * @brief Check if token is an operator
 */
bool isOperator(TokenType type);

/**
 * @brief Check if token is a literal
 */
bool isLiteral(TokenType type);

/**
 * @brief Get keyword token type from string
 */
TokenType getKeywordType(const std::string& keyword);

} // namespace CHTL

#endif // CHTL_TOKEN_H
