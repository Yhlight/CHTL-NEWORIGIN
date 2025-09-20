#pragma once

#include "../../../include/CHTLCommon.h"
#include "../../../include/Token.h"
#include <regex>

namespace CHTL {

class CHTLLexer {
private:
    String source;
    String filename;
    size_t position;
    int line;
    int column;
    TokenVector tokens;
    
    // Lexer state
    bool inComment;
    bool inString;
    char stringDelimiter;
    bool inGeneratorComment;
    
    // Regular expressions for token recognition
    std::regex identifierRegex;
    std::regex numberRegex;
    std::regex stringRegex;
    std::regex commentRegex;
    std::regex generatorCommentRegex;
    
    // Keywords and operators
    StringMap keywords;
    StringMap operators;
    StringMap brackets;
    
public:
    CHTLLexer();
    CHTLLexer(const String& src, const String& file = "");
    
    // Main lexing methods
    TokenVector tokenize();
    TokenVector tokenizeFile(const String& filepath);
    
    // Character and position management
    char currentChar() const;
    char peekChar(int offset = 1) const;
    void advance(int steps = 1);
    void skipWhitespace();
    bool isAtEnd() const;
    
    // Token recognition methods
    std::shared_ptr<Token> scanIdentifier();
    std::shared_ptr<Token> scanString();
    std::shared_ptr<Token> scanNumber();
    std::shared_ptr<Token> scanComment();
    std::shared_ptr<Token> scanGeneratorComment();
    std::shared_ptr<Token> scanOperator();
    std::shared_ptr<Token> scanBracket();
    
    // Special token recognition
    std::shared_ptr<Token> scanCEEquivalence(); // Colon = Equal equivalence
    std::shared_ptr<Token> scanLiteral(); // Unquoted literals
    std::shared_ptr<Token> scanAttributeExpression(); // CSS-like expressions
    
    // Helper methods
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isNewline(char c) const;
    bool isOperatorChar(char c) const;
    bool isBracketChar(char c) const;
    bool isStringDelimiter(char c) const;
    
    // Keyword and operator checking
    bool isKeyword(const String& str) const;
    bool isOperator(const String& str) const;
    bool isBracket(const String& str) const;
    TokenType getKeywordType(const String& str) const;
    TokenType getOperatorType(const String& str) const;
    TokenType getBracketType(const String& str) const;
    
    // State management
    void reset();
    void setSource(const String& src, const String& file = "");
    const String& getSource() const { return source; }
    const String& getFilename() const { return filename; }
    size_t getPosition() const { return position; }
    int getLine() const { return line; }
    int getColumn() const { return column; }
    
    // Error handling
    void reportError(const String& message, int line = -1, int col = -1);
    
private:
    void initializeKeywords();
    void initializeOperators();
    void initializeBrackets();
    void initializeRegex();
    
    std::shared_ptr<Token> createToken(TokenType type, const String& value);
    void updatePosition(char c);
};

} // namespace CHTL