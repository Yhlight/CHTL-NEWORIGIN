#pragma once

#include "CHTL/CHTLLexer/Token.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace CHTL {

/**
 * @brief CHTL Lexer for tokenizing CHTL source code
 * 
 * The lexer handles all CHTL syntax features including:
 * - Comments (//, block comments, #)
 * - Text nodes and literals
 * - Elements and attributes
 * - Style blocks and property operations
 * - Template and custom definitions
 * - Import statements and namespaces
 * - CHTL JS syntax
 */
class Lexer {
public:
    Lexer();
    ~Lexer() = default;

    // Main lexing function
    std::vector<Token> tokenize(const std::string& source);
    std::vector<Token> tokenizeFile(const std::string& filePath);

    // Configuration
    void setCaseSensitive(bool caseSensitive) { caseSensitive_ = caseSensitive; }
    void setPreserveWhitespace(bool preserve) { preserveWhitespace_ = preserve; }
    void setPreserveComments(bool preserve) { preserveComments_ = preserve; }

    // Error handling
    bool hasError() const { return hasError_; }
    const std::string& getErrorMessage() const { return errorMessage_; }
    size_t getErrorLine() const { return errorLine_; }
    size_t getErrorColumn() const { return errorColumn_; }

private:
    // Lexing state
    std::string source_;
    size_t position_;
    size_t line_;
    size_t column_;
    bool hasError_;
    std::string errorMessage_;
    size_t errorLine_;
    size_t errorColumn_;

    // Configuration
    bool caseSensitive_;
    bool preserveWhitespace_;
    bool preserveComments_;

    // Keyword mapping
    std::unordered_map<std::string, TokenType> keywords_;

    // Core lexing methods
    void initializeKeywords();
    void reset();
    void setError(const std::string& message);
    
    // Character utilities
    char current() const;
    char peek(size_t offset = 1) const;
    bool advance();
    bool isAtEnd() const;
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isNewline(char c) const;
    bool isQuote(char c) const;
    bool isOperator(char c) const;
    bool isBracket(char c) const;
    
    // Token creation
    Token createToken(TokenType type, const std::string& value);
    Token createToken(TokenType type, char value);
    
    // Token recognition
    Token scanIdentifier();
    Token scanNumber();
    Token scanString(char quote);
    Token scanUnquotedLiteral();
    Token scanComment();
    Token scanOperator();
    Token scanBracket();
    Token scanWhitespace();
    Token scanNewline();
    
    // Special CHTL syntax recognition
    Token scanTemplateKeyword();
    Token scanCustomKeyword();
    Token scanImportKeyword();
    Token scanNamespaceKeyword();
    Token scanConfigurationKeyword();
    Token scanOriginKeyword();
    Token scanGeneratorComment();
    
    // CHTL JS syntax recognition
    Token scanCHTLJSKeyword();
    Token scanCSSUnit();
    Token scanCSSFunction();
    Token scanCSSSelector();
    Token scanCSSPseudo();
    
    // Utility methods
    void skipWhitespace();
    bool match(char expected);
    bool matchSequence(const std::string& sequence);
    std::string extractString(size_t start, size_t end);
    bool isKeyword(const std::string& word) const;
    TokenType getKeywordType(const std::string& word) const;
    
    // Multi-character operator recognition
    TokenType recognizeOperator(const std::string& op) const;
    bool isMultiCharOperator(char first, char second) const;
    
    // Literal recognition
    bool isUnquotedLiteralStart(char c) const;
    bool isUnquotedLiteralChar(char c) const;
    bool isNumberStart(char c) const;
    bool isNumberChar(char c) const;
    
    // CSS/JS specific recognition
    bool isCSSUnit(const std::string& value) const;
    bool isCSSFunction(const std::string& value) const;
    bool isCSSSelector(const std::string& value) const;
    bool isCSSPseudo(const std::string& value) const;
};

} // namespace CHTL