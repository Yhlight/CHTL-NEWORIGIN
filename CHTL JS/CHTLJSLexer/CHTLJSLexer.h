#ifndef CHTL_JS_LEXER_H
#define CHTL_JS_LEXER_H

#include <string>
#include <vector>
#include <map>

// CHTL JS Token类型枚举
enum class CHTLJSTokenType {
    // 关键字
    LISTEN,         // Listen
    ANIMATE,        // Animate
    ROUTER,         // Router
    VIR,            // Vir
    INEVERAWAY,     // iNeverAway
    PRINTMYLOVE,    // printMylove
    SCRIPTLOADER,   // ScriptLoader
    
    // 标识符
    IDENTIFIER,     // 标识符
    FUNCTION,       // 函数名
    
    // 字面量
    STRING,         // 字符串
    NUMBER,         // 数字
    BOOLEAN,        // 布尔值
    
    // 运算符
    ARROW,          // ->
    AMPERSAND_ARROW, // &->
    DOT,            // .
    COMMA,          // ,
    COLON,          // :
    SEMICOLON,      // ;
    
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
    
    // 选择器
    SELECTOR,       // {{选择器}}
    
    // 其他
    TEXT,           // 文本内容
    WHITESPACE,     // 空白字符
    NEWLINE,        // 换行符
    EOF_TOKEN,      // 文件结束
    
    // 错误
    ERROR           // 错误token
};

// CHTL JS Token结构
struct CHTLJSToken {
    CHTLJSTokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t length;
    
    CHTLJSToken(CHTLJSTokenType t = CHTLJSTokenType::ERROR, const std::string& v = "", 
                size_t l = 0, size_t c = 0, size_t len = 0)
        : type(t), value(v), line(l), column(c), length(len) {}
};

class CHTLJSLexer {
public:
    CHTLJSLexer();
    ~CHTLJSLexer() = default;
    
    // 主要接口
    bool tokenize(const std::string& input, std::vector<CHTLJSToken>& tokens);
    
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
    std::map<std::string, CHTLJSTokenType> keywords_;
    
    // 初始化
    void initializeKeywords();
    
    // 主要扫描方法
    bool scanTokens(std::vector<CHTLJSToken>& tokens);
    CHTLJSToken scanNextToken();
    
    // 具体token扫描方法
    CHTLJSToken scanIdentifier();
    CHTLJSToken scanString();
    CHTLJSToken scanNumber();
    CHTLJSToken scanBoolean();
    CHTLJSToken scanSelector();
    CHTLJSToken scanOperator();
    CHTLJSToken scanDelimiter();
    CHTLJSToken scanSpecial();
    CHTLJSToken scanText();
    CHTLJSToken scanWhitespace();
    
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
    CHTLJSToken createErrorToken(const std::string& message);
};

#endif // CHTL_JS_LEXER_H