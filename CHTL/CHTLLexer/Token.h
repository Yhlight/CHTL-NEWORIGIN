#pragma once

#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include <memory>

namespace CHTL {

enum class TokenType {
    // 基础类型
    IDENTIFIER,     // 标识符
    STRING,         // 字符串
    NUMBER,         // 数字
    COMMENT,        // 注释
    
    // 关键字
    KEYWORD_HTML,   // html
    KEYWORD_HEAD,   // head
    KEYWORD_BODY,   // body
    KEYWORD_DIV,    // div
    KEYWORD_SPAN,   // span
    KEYWORD_TEXT,   // text
    KEYWORD_STYLE,  // style
    KEYWORD_SCRIPT, // script
    
    // 模板相关关键字
    KEYWORD_TEMPLATE,   // [Template]
    KEYWORD_CUSTOM,     // [Custom]
    KEYWORD_ORIGIN,     // [Origin]
    KEYWORD_IMPORT,     // [Import]
    KEYWORD_NAMESPACE,  // [Namespace]
    KEYWORD_CONFIGURATION, // [Configuration]
    KEYWORD_INFO,       // [Info]
    KEYWORD_EXPORT,     // [Export]
    
    // 模板类型
    TEMPLATE_STYLE,     // @Style
    TEMPLATE_ELEMENT,   // @Element
    TEMPLATE_VAR,       // @Var
    TEMPLATE_HTML,      // @Html
    TEMPLATE_JAVASCRIPT, // @JavaScript
    TEMPLATE_CHTL,      // @Chtl
    TEMPLATE_CJMOD,     // @CJmod
    TEMPLATE_CONFIG,    // @Config
    
    // 操作符
    ASSIGN,         // =
    COLON,          // :
    SEMICOLON,      // ;
    COMMA,          // ,
    DOT,            // .
    QUESTION,       // ?
    EXCLAMATION,    // !
    
    // 算术操作符
    PLUS,           // +
    MINUS,          // -
    MULTIPLY,       // *
    DIVIDE,         // /
    MODULO,         // %
    POWER,          // **
    
    // 比较操作符
    EQUAL,          // ==
    NOT_EQUAL,      // !=
    LESS,           // <
    LESS_EQUAL,     // <=
    GREATER,        // >
    GREATER_EQUAL,  // >=
    
    // 逻辑操作符
    AND,            // &&
    OR,             // ||
    NOT,            // !
    
    // 括号
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )
    LEFT_BRACKET,   // [
    RIGHT_BRACKET,  // ]
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    
    // 特殊符号
    HASH,           // #
    AT,             // @
    DOLLAR,         // $
    AMPERSAND,      // &
    PIPE,           // |
    TILDE,          // ~
    CARET,          // ^
    
    // 字符串分隔符
    SINGLE_QUOTE,   // '
    DOUBLE_QUOTE,   // "
    BACKTICK,       // `
    
    // 其他
    NEWLINE,        // 换行符
    WHITESPACE,     // 空白字符
    EOF_TOKEN,      // 文件结束
    UNKNOWN         // 未知字符
};

struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t position;
    
    Token() : type(TokenType::UNKNOWN), line(0), column(0), position(0) {}
    Token(TokenType t, const std::string& v, size_t l, size_t c, size_t p)
        : type(t), value(v), line(l), column(c), position(p) {}
    
    // 检查是否为特定类型
    bool is(TokenType t) const { return type == t; }
    bool isOneOf(const std::vector<TokenType>& types) const;
    
    // 检查是否为关键字
    bool isKeyword() const;
    
    // 检查是否为操作符
    bool isOperator() const;
    
    // 检查是否为括号
    bool isBracket() const;
    
    // 检查是否为算术操作符
    bool isArithmeticOperator() const;
    
    // 检查是否为比较操作符
    bool isComparisonOperator() const;
    
    // 检查是否为逻辑操作符
    bool isLogicalOperator() const;
    
    // 获取操作符优先级
    int getPrecedence() const;
    
    // 获取操作符结合性（true为左结合，false为右结合）
    bool isLeftAssociative() const;
    
    // 转换为字符串
    std::string toString() const;
    
    // 检查是否为空或无效
    bool isEmpty() const;
    
    // 检查是否为空白字符
    bool isWhitespace() const;
    
    // 检查是否为换行符
    bool isNewline() const;
    
    // 检查是否为文件结束
    bool isEOF() const;
};

// Token流类
class TokenStream {
public:
    TokenStream(const std::vector<Token>& tokens);
    
    // 获取当前token
    const Token& current() const;
    
    // 获取下一个token
    const Token& next();
    
    // 查看下一个token但不移动位置
    const Token& peek() const;
    
    // 查看前一个token
    const Token& previous() const;
    
    // 检查是否到达末尾
    bool isEOF() const;
    
    // 检查是否还有更多token
    bool hasMore() const;
    
    // 获取当前位置
    size_t position() const;
    
    // 设置位置
    void setPosition(size_t pos);
    
    // 跳过空白字符和注释
    void skipWhitespace();
    
    // 跳过换行符
    void skipNewlines();
    
    // 期望特定类型的token
    bool expect(TokenType type);
    bool expect(const std::vector<TokenType>& types);
    
    // 期望特定值的token
    bool expectValue(const std::string& value);
    
    // 获取所有token
    const std::vector<Token>& getAllTokens() const;
    
    // 重置到开始位置
    void reset();
    
private:
    std::vector<Token> tokens_;
    size_t currentPos_;
};

// Token工具函数
class TokenUtil {
public:
    // 将TokenType转换为字符串
    static std::string tokenTypeToString(TokenType type);
    
    // 将字符串转换为TokenType
    static TokenType stringToTokenType(const std::string& str);
    
    // 检查字符串是否为关键字
    static bool isKeyword(const std::string& str);
    
    // 检查字符串是否为操作符
    static bool isOperator(const std::string& str);
    
    // 获取关键字的TokenType
    static TokenType getKeywordType(const std::string& str);
    
    // 获取操作符的TokenType
    static TokenType getOperatorType(const std::string& str);
    
    // 检查字符是否为操作符
    static bool isOperatorChar(char c);
    
    // 检查字符是否为标识符字符
    static bool isIdentifierChar(char c);
    
    // 检查字符是否为标识符起始字符
    static bool isIdentifierStartChar(char c);
    
    // 检查字符是否为数字
    static bool isDigit(char c);
    
    // 检查字符是否为十六进制数字
    static bool isHexDigit(char c);
    
    // 检查字符是否为空白字符
    static bool isWhitespace(char c);
    
    // 检查字符是否为换行符
    static bool isNewline(char c);
    
    // 检查字符是否为字符串分隔符
    static bool isStringDelimiter(char c);
    
    // 检查字符是否为注释开始
    static bool isCommentStart(char c, char next);
    
    // 获取操作符优先级
    static int getOperatorPrecedence(TokenType type);
    
    // 获取操作符结合性
    static bool isLeftAssociative(TokenType type);
};

} // namespace CHTL