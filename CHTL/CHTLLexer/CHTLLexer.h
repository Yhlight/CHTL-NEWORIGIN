#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include <string>
#include <vector>
#include <map>

// Token类型枚举
enum class TokenType {
    // 关键字
    TEMPLATE,       // [Template]
    CUSTOM,         // [Custom]
    ORIGIN,         // [Origin]
    IMPORT,         // [Import]
    CONFIGURATION,  // [Configuration]
    NAMESPACE,      // [Namespace]
    INFO,           // [Info]
    EXPORT,         // [Export]
    
    // 标识符
    IDENTIFIER,     // 标识符
    ELEMENT,        // HTML元素
    ATTRIBUTE,      // 属性名
    
    // 字面量
    STRING,         // 字符串
    SINGLE_QUOTE_STRING, // 单引号字符串
    NUMBER,         // 数字
    LITERAL,        // 无修饰字面量
    TEXT_CONTENT,   // 文本内容
    
    // 运算符
    COLON,          // :
    EQUAL,          // =
    SEMICOLON,      // ;
    COMMA,          // ,
    DOT,            // .
    SLASH,          // /
    
    // 分隔符
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    LEFT_BRACKET,   // [
    RIGHT_BRACKET,  // ]
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )
    
    // 特殊符号
    AT,             // @
    HASH,           // #
    AMPERSAND,      // &
    DOLLAR,         // $
    QUESTION,       // ?
    EXCLAMATION,    // !
    UNDERSCORE,     // _
    
    // 注释
    SINGLE_LINE_COMMENT, // // 注释
    MULTI_LINE_COMMENT,  // /* */ 注释
    GENERATOR_COMMENT,   // # 注释
    
    // 其他
    TEXT,           // 文本内容
    WHITESPACE,     // 空白字符
    NEWLINE,        // 换行符
    EOF_TOKEN,      // 文件结束
    
    // 错误
    ERROR           // 错误token
};

// Token结构
struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t length;
    
    Token(TokenType t = TokenType::ERROR, const std::string& v = "", 
          size_t l = 0, size_t c = 0, size_t len = 0)
        : type(t), value(v), line(l), column(c), length(len) {}
};

class CHTLLexer {
public:
    CHTLLexer();
    ~CHTLLexer() = default;
    
    // 主要接口
    bool tokenize(const std::string& input, std::vector<Token>& tokens);
    
    // 设置选项
    void setDebugMode(bool enabled) { debugMode_ = enabled; }
    
private:
    // 词法分析状态
    size_t currentPos_;
    size_t currentLine_;
    size_t currentColumn_;
    std::string input_;
    
    // 选项
    bool debugMode_;
    
    // 关键字映射
    std::map<std::string, TokenType> keywords_;
    
    // 初始化
    void initializeKeywords();
    
    // 主要扫描方法
    bool scanTokens(std::vector<Token>& tokens);
    Token scanNextToken();
    
    // 具体token扫描方法
    Token scanIdentifier();
    Token scanString();
    Token scanSingleQuoteString();
    Token scanNumber();
    Token scanLiteral();
    Token scanSingleLineComment();
    Token scanMultiLineComment();
    Token scanGeneratorComment();
    Token scanOperator();
    Token scanDelimiter();
    Token scanSpecial();
    Token scanText();
    Token scanWhitespace();
    
    // 辅助方法
    char currentChar() const;
    char peekChar(size_t offset = 1) const;
    void advance(size_t count = 1);
    bool isAtEnd() const;
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isNewline(char c) const;
    
    // 跳过空白字符
    void skipWhitespace();
    
    // 错误处理
    Token createErrorToken(const std::string& message);
};

#endif // CHTL_LEXER_H