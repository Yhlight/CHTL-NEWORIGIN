#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

/**
 * @brief Token types for CHTL language
 */
enum class TokenType {
    // Basic tokens
    IDENTIFIER,
    STRING_LITERAL,
    NUMBER_LITERAL,
    UNQUOTED_LITERAL,
    
    // Operators
    COLON,
    EQUALS,
    SEMICOLON,
    COMMA,
    DOT,
    HASH,
    AT,
    QUESTION,
    EXCLAMATION,
    
    // Brackets and braces
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_ANGLE,
    RIGHT_ANGLE,
    
    // Arithmetic operators
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    MODULO,
    POWER,
    
    // Comparison operators
    EQUAL,
    NOT_EQUAL,
    LESS_THAN,
    LESS_EQUAL,
    GREATER_THAN,
    GREATER_EQUAL,
    
    // Logical operators
    AND,
    OR,
    NOT,
    
    // Keywords
    TEXT,
    STYLE,
    SCRIPT,
    TEMPLATE,
    CUSTOM,
    ORIGIN,
    IMPORT,
    CONFIG,
    NAMESPACE,
    INHERIT,
    DELETE,
    INSERT,
    AFTER,
    BEFORE,
    REPLACE,
    AT_TOP,
    AT_BOTTOM,
    FROM,
    AS,
    EXCEPT,
    USE,
    HTML5,
    
    // Special tokens
    COMMENT,
    MULTILINE_COMMENT,
    GENERATOR_COMMENT,
    WHITESPACE,
    NEWLINE,
    EOF_TOKEN,
    ERROR
};

/**
 * @brief Token structure
 */
struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
    
    Token(TokenType t = TokenType::EOF_TOKEN, const std::string& v = "", size_t l = 0, size_t c = 0)
        : type(t), value(v), line(l), column(c) {}
    
    bool operator==(const Token& other) const {
        return type == other.type && value == other.value;
    }
    
    std::string toString() const;
};

/**
 * @brief Global map for managing identifiers and keywords
 */
class GlobalMap {
public:
    GlobalMap();
    ~GlobalMap() = default;
    
    /**
     * @brief Add identifier to global map
     * @param identifier Identifier name
     * @param value Identifier value
     */
    void addIdentifier(const std::string& identifier, const std::string& value);
    
    /**
     * @brief Get identifier value
     * @param identifier Identifier name
     * @return Identifier value or empty string if not found
     */
    std::string getIdentifier(const std::string& identifier) const;
    
    /**
     * @brief Check if identifier exists
     * @param identifier Identifier name
     * @return True if exists, false otherwise
     */
    bool hasIdentifier(const std::string& identifier) const;
    
    /**
     * @brief Remove identifier
     * @param identifier Identifier name
     */
    void removeIdentifier(const std::string& identifier);
    
    /**
     * @brief Clear all identifiers
     */
    void clear();
    
    /**
     * @brief Get all identifiers
     * @return Map of all identifiers
     */
    const std::unordered_map<std::string, std::string>& getAllIdentifiers() const;

private:
    std::unordered_map<std::string, std::string> identifiers_;
};

/**
 * @brief CHTL Lexer - Tokenizes CHTL input
 * 
 * Handles all CHTL syntax features including:
 * - Comments (//, /* * /, #)
 * - Text nodes and literals
 * - Element parsing
 * - Attribute parsing
 * - Style block parsing
 * - Script block parsing
 * - Template parsing
 * - Custom parsing
 * - Import parsing
 * - Configuration parsing
 * - Namespace parsing
 * - Operator parsing
 */
class CHTLLexer {
public:
    CHTLLexer();
    ~CHTLLexer() = default;
    
    /**
     * @brief Initialize lexer
     */
    void initialize();
    
    /**
     * @brief Tokenize input string
     * @param input Input string to tokenize
     * @return Vector of tokens
     */
    std::vector<Token> tokenize(const std::string& input);
    
    /**
     * @brief Tokenize file
     * @param filePath Path to file to tokenize
     * @return Vector of tokens
     */
    std::vector<Token> tokenizeFile(const std::string& filePath);
    
    /**
     * @brief Get current token
     * @return Current token
     */
    const Token& getCurrentToken() const;
    
    /**
     * @brief Advance to next token
     * @return True if successful, false if at end
     */
    bool advance();
    
    /**
     * @brief Peek at next token without advancing
     * @return Next token
     */
    Token peek() const;
    
    /**
     * @brief Check if at end of input
     * @return True if at end
     */
    bool isAtEnd() const;
    
    /**
     * @brief Reset lexer state
     */
    void reset();
    
    /**
     * @brief Get global map
     * @return Reference to global map
     */
    GlobalMap& getGlobalMap();
    
    /**
     * @brief Get error message
     * @return Error message
     */
    std::string getErrorMessage() const;

private:
    std::string input_;
    size_t position_;
    size_t line_;
    size_t column_;
    Token currentToken_;
    std::vector<Token> tokens_;
    size_t currentTokenIndex_;
    GlobalMap globalMap_;
    std::string errorMessage_;
    
    // Tokenization methods
    void tokenizeInput();
    Token getNextToken();
    void skipWhitespace();
    void skipComment();
    void skipMultilineComment();
    Token parseStringLiteral();
    Token parseNumberLiteral();
    Token parseIdentifier();
    Token parseOperator();
    Token parseSpecial();
    
    // Helper methods
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isOperator(char c) const;
    bool isSpecial(char c) const;
    char peekChar() const;
    char peekChar(size_t offset) const;
    char advanceChar();
    bool matchChar(char expected);
    void addToken(TokenType type, const std::string& value = "");
    
    // Keyword recognition
    TokenType getKeywordType(const std::string& keyword) const;
    bool isKeyword(const std::string& word) const;
};

} // namespace CHTL