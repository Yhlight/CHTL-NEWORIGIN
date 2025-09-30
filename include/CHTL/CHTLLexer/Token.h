#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace CHTL {

/**
 * @brief Token types for CHTL lexer
 */
enum class TokenType {
    // Basic tokens
    IDENTIFIER,         // Element names, attribute names, etc.
    LITERAL,            // String literals, numbers
    UNQUOTED_LITERAL,   // Unquoted strings (CHTL feature)
    
    // Operators and symbols
    COLON,              // :
    EQUALS,             // =
    SEMICOLON,          // ;
    COMMA,              // ,
    DOT,                // .
    HASH,               // #
    AMPERSAND,          // &
    QUESTION,           // ?
    EXCLAMATION,        // !
    ASTERISK,           // *
    PLUS,               // +
    MINUS,              // -
    SLASH,              // /
    PERCENT,            // %
    CARET,              // ^
    PIPE,               // |
    TILDE,              // ~
    
    // Brackets and braces
    LEFT_PAREN,         // (
    RIGHT_PAREN,        // )
    LEFT_BRACKET,       // [
    RIGHT_BRACKET,      // ]
    LEFT_BRACE,         // {
    RIGHT_BRACE,        // }
    LEFT_ANGLE,         // <
    RIGHT_ANGLE,        // >
    
    // Comparison operators
    EQUAL_EQUAL,        // ==
    NOT_EQUAL,          // !=
    LESS_EQUAL,         // <=
    GREATER_EQUAL,      // >=
    LESS_THAN,          // <
    GREATER_THAN,       // >
    
    // Logical operators
    AND_AND,            // &&
    OR_OR,              // ||
    NOT,                // !
    
    // Arithmetic operators
    PLUS_PLUS,          // ++
    MINUS_MINUS,        // --
    PLUS_EQUAL,         // +=
    MINUS_EQUAL,        // -=
    MULTIPLY_EQUAL,     // *=
    DIVIDE_EQUAL,       // /=
    MODULO_EQUAL,       // %=
    POWER_EQUAL,        // **=
    
    // Special CHTL tokens
    TEXT_KEYWORD,       // text
    STYLE_KEYWORD,      // style
    SCRIPT_KEYWORD,     // script
    TEMPLATE_KEYWORD,   // [Template]
    CUSTOM_KEYWORD,     // [Custom]
    IMPORT_KEYWORD,     // [Import]
    NAMESPACE_KEYWORD,  // [Namespace]
    CONFIG_KEYWORD,     // [Configuration]
    ORIGIN_KEYWORD,     // [Origin]
    USE_KEYWORD,        // use
    INHERIT_KEYWORD,    // inherit
    DELETE_KEYWORD,     // delete
    INSERT_KEYWORD,     // insert
    EXCEPT_KEYWORD,     // except
    FROM_KEYWORD,       // from
    AS_KEYWORD,         // as
    AT_KEYWORD,         // @
    
    // Comments
    LINE_COMMENT,       // //
    BLOCK_COMMENT,      // /* */
    GENERATOR_COMMENT,  // # (generator comment)
    
    // Whitespace and control
    WHITESPACE,         // spaces, tabs, newlines
    NEWLINE,            // \n
    TAB,                // \t
    SPACE,              // space
    
    // Special values
    STRING_LITERAL,     // "string"
    SINGLE_QUOTE_LITERAL, // 'string'
    NUMBER_LITERAL,     // 123, 123.45
    BOOLEAN_LITERAL,    // true, false
    
    // CSS/JS specific
    CSS_UNIT,           // px, em, rem, %, etc.
    CSS_FUNCTION,       // calc(), rgba(), etc.
    CSS_SELECTOR,       // .class, #id, element
    CSS_PSEUDO,         // :hover, ::before
    
    // CHTL JS specific
    VIR_KEYWORD,        // Vir
    LISTEN_KEYWORD,     // Listen
    ANIMATE_KEYWORD,    // Animate
    ROUTER_KEYWORD,     // Router
    DELEGATE_KEYWORD,   // Delegate
    SCRIPT_LOADER_KEYWORD, // ScriptLoader
    
    // End of file
    EOF_TOKEN,          // End of file
    
    // Error
    ERROR_TOKEN         // Lexical error
};

/**
 * @brief Token class representing a lexical token
 */
class Token {
public:
    Token(TokenType type, const std::string& value, size_t line = 0, size_t column = 0);
    Token(const Token& other) = default;
    Token& operator=(const Token& other) = default;
    Token(Token&& other) noexcept = default;
    Token& operator=(Token&& other) noexcept = default;
    ~Token() = default;

    // Getters
    TokenType getType() const { return type_; }
    const std::string& getValue() const { return value_; }
    size_t getLine() const { return line_; }
    size_t getColumn() const { return column_; }
    
    // Setters
    void setType(TokenType type) { type_ = type; }
    void setValue(const std::string& value) { value_ = value; }
    void setPosition(size_t line, size_t column) { line_ = line; column_ = column; }
    
    // Utility methods
    bool isOperator() const;
    bool isKeyword() const;
    bool isLiteral() const;
    bool isWhitespace() const;
    bool isComment() const;
    bool isBracket() const;
    bool isComparisonOperator() const;
    bool isLogicalOperator() const;
    bool isArithmeticOperator() const;
    
    // String representation
    std::string toString() const;
    std::string getTypeString() const;
    
    // Comparison
    bool operator==(const Token& other) const;
    bool operator!=(const Token& other) const;

private:
    TokenType type_;
    std::string value_;
    size_t line_;
    size_t column_;
};

/**
 * @brief Token stream for managing tokens during parsing
 */
class TokenStream {
public:
    TokenStream(const std::vector<Token>& tokens);
    TokenStream(std::vector<Token>&& tokens);
    
    // Token access
    const Token& current() const;
    const Token& peek(size_t offset = 1) const;
    const Token& previous() const;
    
    // Stream navigation
    bool advance();
    bool hasNext() const;
    bool hasPrevious() const;
    size_t getPosition() const;
    void setPosition(size_t position);
    void reset();
    
    // Stream information
    size_t size() const;
    bool empty() const;
    
    // Error handling
    bool hasError() const;
    const Token& getErrorToken() const;

private:
    std::vector<Token> tokens_;
    size_t position_;
    size_t errorPosition_;
    bool hasError_;
};

} // namespace CHTL