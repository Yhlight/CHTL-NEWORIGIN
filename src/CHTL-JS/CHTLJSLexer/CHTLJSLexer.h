#ifndef CHTLJS_LEXER_H
#define CHTLJS_LEXER_H

#include "../../Common.h"
#include "CHTLJSToken.h"
#include <unordered_map>

namespace CHTL {
namespace JS {

// CHTL JS词法分析器
class CHTLJSLexer {
public:
    explicit CHTLJSLexer(const String& source);
    
    // 获取下一个Token
    JSToken nextToken();
    
    // 预览下一个Token
    JSToken peekToken();
    
    // 获取所有Token
    Vector<JSToken> tokenize();
    
    // 是否到达文件末尾
    bool isAtEnd() const { return current >= source.length(); }
    
private:
    // 初始化关键字映射
    void initializeKeywords();
    
    // 字符读取
    char advance();
    char peek() const;
    char peekNext() const;
    bool match(char expected);
    
    // 跳过空白
    void skipWhitespace();
    
    // Token扫描
    JSToken scanToken();
    JSToken scanIdentifierOrKeyword();
    JSToken scanStringLiteral(char quote);
    JSToken scanNumberLiteral();
    JSToken scanEnhancedSelector();
    
    // 辅助函数
    bool isWhitespace(char c) const;
    bool isDigit(char c) const;
    bool isAlpha(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isIdentifierStart(char c) const;
    bool isIdentifierChar(char c) const;
    
    // 创建Token
    JSToken makeToken(JSTokenType type, const String& value);
    JSToken makeToken(JSTokenType type);
    
    // 错误处理
    void error(const String& message);
    
    // 成员变量
    String source;
    size_t current = 0;
    Position currentPos;
    Position tokenStart;
    
    // 关键字映射
    std::unordered_map<String, JSTokenType> keywords;
    
    // 缓存的Token
    Optional<JSToken> cachedToken;
};

} // namespace JS
} // namespace CHTL

#endif // CHTLJS_LEXER_H
