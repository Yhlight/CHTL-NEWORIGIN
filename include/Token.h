#pragma once

#include "CHTLCommon.h"

namespace CHTL {

class Token {
private:
    TokenType type;
    String value;
    int line;
    int column;
    String filename;

public:
    Token(TokenType t, const String& v, int l, int c, const String& f = "")
        : type(t), value(v), line(l), column(c), filename(f) {}
    
    // Getters
    TokenType getType() const { return type; }
    const String& getValue() const { return value; }
    int getLine() const { return line; }
    int getColumn() const { return column; }
    const String& getFilename() const { return filename; }
    
    // Setters
    void setType(TokenType t) { type = t; }
    void setValue(const String& v) { value = v; }
    void setPosition(int l, int c) { line = l; column = c; }
    void setFilename(const String& f) { filename = f; }
    
    // Utility methods
    bool isKeyword() const;
    bool isOperator() const;
    bool isBracket() const;
    bool isLiteral() const;
    bool isIdentifier() const;
    
    String toString() const;
    String getTypeString() const;
    
    // Comparison operators
    bool operator==(const Token& other) const;
    bool operator!=(const Token& other) const;
};

// Token utility functions
class TokenUtil {
public:
    static TokenType stringToTokenType(const String& str);
    static String tokenTypeToString(TokenType type);
    static bool isKeyword(const String& str);
    static bool isOperator(const String& str);
    static bool isBracket(const String& str);
    static TokenType getOperatorType(const String& op);
    static int getOperatorPrecedence(TokenType type);
    static bool isLeftAssociative(TokenType type);
};

} // namespace CHTL