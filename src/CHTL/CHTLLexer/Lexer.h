#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "../../Common.h"
#include "Token.h"
#include "GlobalMap.h"

namespace CHTL {

// 词法分析器
class Lexer {
public:
    explicit Lexer(const String& source);
    
    // 获取下一个Token
    Token nextToken();
    
    // 预览下一个Token（不消耗）
    Token peekToken();
    
    // 获取所有Token
    Vector<Token> tokenize();
    
    // 获取当前位置
    const Position& getCurrentPosition() const { return currentPos; }
    
    // 是否到达文件末尾
    bool isAtEnd() const { return current >= source.length(); }
    
private:
    // 字符读取
    char advance();
    char peek() const;
    char peekNext() const;
    bool match(char expected);
    
    // 跳过空白
    void skipWhitespace();
    
    // Token扫描
    Token scanToken();
    Token scanIdentifierOrKeyword();
    Token scanStringLiteral(char quote);
    Token scanNumberLiteral();
    Token scanUnquotedLiteral();
    Token scanComment();
    Token scanGeneratorComment();
    Token scanEnhancedSelector();
    
    // 辅助函数
    bool isWhitespace(char c) const;
    bool isDigit(char c) const;
    bool isAlpha(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isIdentifierStart(char c) const;
    bool isIdentifierChar(char c) const;
    
    // 创建Token
    Token makeToken(TokenType type, const String& value);
    Token makeToken(TokenType type);
    
    // 错误处理
    void error(const String& message);
    
    // 成员变量
    String source;
    size_t current = 0;
    Position currentPos;
    Position tokenStart;
    GlobalMap globalMap;
    
    // 缓存的Token（用于peek）
    Optional<Token> cachedToken;
};

} // namespace CHTL

#endif // CHTL_LEXER_H
