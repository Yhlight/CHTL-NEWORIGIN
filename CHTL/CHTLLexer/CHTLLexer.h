#pragma once

#include "Token.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

class CHTLLexer {
public:
    CHTLLexer();
    ~CHTLLexer();
    
    // 词法分析主函数
    std::vector<Token> tokenize(const std::string& input);
    
    // 从文件读取并分析
    std::vector<Token> tokenizeFile(const std::string& filename);
    
    // 设置选项
    void setSkipWhitespace(bool skip);
    void setSkipComments(bool skip);
    void setCaseSensitive(bool sensitive);
    
    // 获取选项
    bool isSkipWhitespace() const;
    bool isSkipComments() const;
    bool isCaseSensitive() const;
    
    // 错误处理
    struct LexerError {
        size_t line;
        size_t column;
        std::string message;
        std::string context;
    };
    
    const std::vector<LexerError>& getErrors() const;
    bool hasErrors() const;
    void clearErrors();
    
private:
    // 输入相关
    std::string input_;
    size_t position_;
    size_t line_;
    size_t column_;
    
    // 选项
    bool skipWhitespace_;
    bool skipComments_;
    bool caseSensitive_;
    
    // 错误处理
    std::vector<LexerError> errors_;
    
    // 核心分析函数
    Token nextToken();
    Token readIdentifier();
    Token readString();
    Token readNumber();
    Token readComment();
    Token readOperator();
    Token readPunctuation();
    Token readWhitespace();
    Token readNewline();
    
    // 辅助函数
    char currentChar() const;
    char peekChar() const;
    char nextChar();
    void advance();
    void skipWhitespace();
    
    // 字符分类
    bool isIdentifierStart(char c) const;
    bool isIdentifierChar(char c) const;
    bool isDigit(char c) const;
    bool isHexDigit(char c) const;
    bool isWhitespace(char c) const;
    bool isNewline(char c) const;
    bool isStringDelimiter(char c) const;
    bool isCommentStart(char c) const;
    bool isOperatorChar(char c) const;
    bool isPunctuationChar(char c) const;
    
    // 字符串处理
    std::string readStringLiteral(char delimiter);
    std::string readRawString();
    std::string readTemplateString();
    
    // 数字处理
    std::string readInteger();
    std::string readFloat();
    std::string readHexNumber();
    std::string readBinaryNumber();
    std::string readOctalNumber();
    
    // 标识符处理
    std::string readIdentifierName();
    TokenType classifyIdentifier(const std::string& identifier);
    
    // 操作符处理
    std::string readOperatorSequence();
    TokenType classifyOperator(const std::string& op);
    
    // 注释处理
    std::string readLineComment();
    std::string readBlockComment();
    
    // 错误报告
    void reportError(const std::string& message);
    void reportError(const std::string& message, size_t line, size_t column);
    std::string getContext(size_t line, size_t column) const;
    
    // 位置管理
    void updatePosition(char c);
    void resetPosition();
    
    // 状态管理
    void reset();
    void setInput(const std::string& input);
    
    // 工具函数
    std::string toCase(const std::string& str) const;
    bool isKeyword(const std::string& str) const;
    bool isTemplateKeyword(const std::string& str) const;
    bool isOperatorKeyword(const std::string& str) const;
    
    // 预定义关键字映射
    static std::unordered_map<std::string, TokenType> keywordMap_;
    static std::unordered_map<std::string, TokenType> templateMap_;
    static std::unordered_map<std::string, TokenType> operatorMap_;
    
    // 初始化静态映射
    static void initializeMaps();
    static bool mapsInitialized_;
};

} // namespace CHTL