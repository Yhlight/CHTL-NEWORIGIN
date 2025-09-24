#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

// Token types for CHTL
enum class TokenType {
    // Literals
    IDENTIFIER,
    STRING_LITERAL,
    UNQUOTED_LITERAL,
    NUMERIC_LITERAL,
    
    // Keywords
    KEYWORD_USE,
    KEYWORD_HTML5,
    KEYWORD_TEXT,
    KEYWORD_STYLE,
    KEYWORD_SCRIPT,
    KEYWORD_CUSTOM,
    KEYWORD_TEMPLATE,
    KEYWORD_ORIGIN,
    KEYWORD_IMPORT,
    KEYWORD_NAMESPACE,
    KEYWORD_INHERIT,
    KEYWORD_DELETE,
    KEYWORD_INSERT,
    KEYWORD_AFTER,
    KEYWORD_BEFORE,
    KEYWORD_REPLACE,
    KEYWORD_ATTOP,
    KEYWORD_ATBOTTOM,
    KEYWORD_FROM,
    KEYWORD_AS,
    KEYWORD_EXCEPT,
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_ELSEIF,
    
    // Operators
    OPERATOR_COLON,
    OPERATOR_EQUALS,
    OPERATOR_PLUS,
    OPERATOR_MINUS,
    OPERATOR_MULTIPLY,
    OPERATOR_DIVIDE,
    OPERATOR_MODULO,
    OPERATOR_POWER,
    OPERATOR_AND,
    OPERATOR_OR,
    OPERATOR_NOT,
    OPERATOR_LESS,
    OPERATOR_GREATER,
    OPERATOR_LESS_EQUAL,
    OPERATOR_GREATER_EQUAL,
    OPERATOR_EQUAL,
    OPERATOR_NOT_EQUAL,
    OPERATOR_ARROW,
    OPERATOR_DOT,
    OPERATOR_COMMA,
    OPERATOR_SEMICOLON,
    OPERATOR_QUESTION,
    
    // Brackets and delimiters
    BRACE_OPEN,
    BRACE_CLOSE,
    PAREN_OPEN,
    PAREN_CLOSE,
    BRACKET_OPEN,
    BRACKET_CLOSE,
    ANGLE_OPEN,
    ANGLE_CLOSE,
    
    // Special tokens
    AT_SYMBOL,
    HASH_SYMBOL,
    DOLLAR_SYMBOL,
    AMPERSAND_SYMBOL,
    PERCENT_SYMBOL,
    
    // Comments
    LINE_COMMENT,
    BLOCK_COMMENT,
    GENERATOR_COMMENT,
    
    // Whitespace
    WHITESPACE,
    NEWLINE,
    TAB,
    
    // End of file
    EOF_TOKEN,
    
    // Error
    ERROR_TOKEN,
    
    // CHTL-specific tokens
    CHTL_ELEMENT,
    CHTL_ATTRIBUTE,
    CHTL_BEHAVIOR,
    CHTL_TEMPLATE_CALL,
    CHTL_CUSTOM_CALL,
    CHTL_VAR_CALL,
    CHTL_ORIGIN_CALL,
    CHTL_IMPORT_CALL,
    CHTL_NAMESPACE_CALL,
    CHTL_CONFIG_CALL,
    
    // CJMOD-specific tokens (for CHTL JS)
    CJMOD_DOUBLE_BRACE_OPEN,
    CJMOD_DOUBLE_BRACE_CLOSE,
    CJMOD_VIRTUAL_OBJECT,
    CJMOD_FUNCTION_CALL,
    CJMOD_SELECTOR,
    CJMOD_LISTEN,
    CJMOD_ANIMATE,
    CJMOD_ROUTER,
    CJMOD_DELEGATE
};

// Token class
class Token {
public:
    Token(TokenType type = TokenType::EOF_TOKEN, const std::string& value = "", 
          size_t line = 0, size_t column = 0);
    
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
    bool isKeyword() const;
    bool isOperator() const;
    bool isLiteral() const;
    bool isDelimiter() const;
    bool isWhitespace() const;
    bool isComment() const;
    bool isError() const;
    
    // Comparison
    bool operator==(const Token& other) const;
    bool operator!=(const Token& other) const;
    
    // String representation
    std::string toString() const;
    std::string getTypeName() const;
    
    // Token classification helpers
    static bool isIdentifierStart(char c);
    static bool isIdentifierPart(char c);
    static bool isDigit(char c);
    static bool isHexDigit(char c);
    static bool isWhitespace(char c);
    static bool isNewline(char c);
    static bool isOperatorChar(char c);
    static bool isDelimiterChar(char c);
    
private:
    TokenType type_;
    std::string value_;
    size_t line_;
    size_t column_;
};

// Token stream for parsing
class TokenStream {
public:
    TokenStream(const std::vector<Token>& tokens);
    TokenStream(std::vector<Token>&& tokens);
    
    // Stream operations
    const Token& peek(size_t offset = 0) const;
    const Token& consume();
    bool hasMore() const;
    size_t remaining() const;
    
    // Position tracking
    size_t getPosition() const { return position_; }
    void setPosition(size_t position);
    void reset();
    
    // Lookahead operations
    bool match(TokenType type, size_t offset = 0) const;
    bool match(const std::string& value, size_t offset = 0) const;
    bool match(TokenType type, const std::string& value, size_t offset = 0) const;
    
    // Utility methods
    std::vector<Token> getRemainingTokens() const;
    std::string getSourceText() const;
    
private:
    std::vector<Token> tokens_;
    size_t position_;
    
    Token eofToken_;
};

// Token factory for creating tokens
class TokenFactory {
public:
    static Token createIdentifier(const std::string& value, size_t line, size_t column);
    static Token createStringLiteral(const std::string& value, size_t line, size_t column);
    static Token createUnquotedLiteral(const std::string& value, size_t line, size_t column);
    static Token createNumericLiteral(const std::string& value, size_t line, size_t column);
    static Token createKeyword(TokenType type, const std::string& value, size_t line, size_t column);
    static Token createOperator(TokenType type, const std::string& value, size_t line, size_t column);
    static Token createDelimiter(TokenType type, size_t line, size_t column);
    static Token createComment(TokenType type, const std::string& value, size_t line, size_t column);
    static Token createWhitespace(const std::string& value, size_t line, size_t column);
    static Token createError(const std::string& value, size_t line, size_t column);
    static Token createEOF(size_t line, size_t column);
    
private:
    static bool isKeywordType(TokenType type);
    static bool isOperatorType(TokenType type);
    static bool isDelimiterType(TokenType type);
    static bool isCommentType(TokenType type);
    static bool isWhitespaceType(TokenType type);
};

// Token utilities
class TokenUtils {
public:
    // Token type to string conversion
    static std::string tokenTypeToString(TokenType type);
    static TokenType stringToTokenType(const std::string& str);
    
    // Token validation
    static bool isValidIdentifier(const std::string& value);
    static bool isValidStringLiteral(const std::string& value);
    static bool isValidNumericLiteral(const std::string& value);
    
    // Token comparison
    static bool tokensEqual(const Token& a, const Token& b, bool ignorePosition = false);
    static bool tokenListEqual(const std::vector<Token>& a, const std::vector<Token>& b, bool ignorePosition = false);
    
    // Token filtering
    static std::vector<Token> filterByType(const std::vector<Token>& tokens, TokenType type);
    static std::vector<Token> filterByTypes(const std::vector<Token>& tokens, const std::vector<TokenType>& types);
    static std::vector<Token> removeWhitespace(const std::vector<Token>& tokens);
    static std::vector<Token> removeComments(const std::vector<Token>& tokens);
    
    // Token formatting
    static std::string formatTokenList(const std::vector<Token>& tokens, bool showPosition = true);
    static std::string formatTokenTable(const std::vector<Token>& tokens);
};

} // namespace CHTL