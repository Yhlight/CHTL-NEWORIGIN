#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include "../../Common.h"

namespace CHTL {

// Token类型枚举
enum class TokenType {
    // 特殊类型
    EndOfFile,
    Unknown,
    
    // 字面量
    Identifier,              // 标识符
    StringLiteral,           // 字符串字面量 "..."
    UnquotedLiteral,         // 无修饰字面量
    NumberLiteral,           // 数字字面量
    
    // 注释
    LineComment,             // // 单行注释
    BlockComment,            // /* */ 块注释
    GeneratorComment,        // # 生成器注释
    
    // 分隔符
    LeftBrace,               // {
    RightBrace,              // }
    LeftBracket,             // [
    RightBracket,            // ]
    LeftParen,               // (
    RightParen,              // )
    Semicolon,               // ;
    Colon,                   // :
    Comma,                   // ,
    Dot,                     // .
    
    // 运算符
    Equal,                   // =
    Plus,                    // +
    Minus,                   // -
    Star,                    // *
    Slash,                   // /
    Percent,                 // %
    Power,                   // **
    
    // 比较运算符
    EqualEqual,              // ==
    NotEqual,                // !=
    Less,                    // <
    Greater,                 // >
    LessEqual,               // <=
    GreaterEqual,            // >=
    
    // 逻辑运算符
    And,                     // &&
    Or,                      // ||
    Not,                     // !
    
    // 特殊运算符
    Question,                // ?
    Arrow,                   // ->
    EventBind,               // &->
    Ampersand,               // &
    At,                      // @
    Hash,                    // #
    Dollar,                  // $
    
    // 关键字 - HTML元素
    HtmlKeyword,             // html, head, body, div, span等
    
    // 关键字 - CHTL语法
    TextKeyword,             // text
    StyleKeyword,            // style
    ScriptKeyword,           // script
    
    // 关键字 - 模板和自定义
    Template,                // [Template]
    Custom,                  // [Custom]
    Origin,                  // [Origin]
    Import,                  // [Import]
    Namespace,               // [Namespace]
    Configuration,           // [Configuration]
    
    // 关键字 - 模板类型
    AtStyle,                 // @Style
    AtElement,               // @Element
    AtVar,                   // @Var
    AtHtml,                  // @Html
    AtJavaScript,            // @JavaScript
    AtChtl,                  // @Chtl
    AtCJmod,                 // @CJmod
    AtConfig,                // @Config
    
    // 关键字 - 操作符
    InheritKeyword,          // inherit
    DeleteKeyword,           // delete
    InsertKeyword,           // insert
    AfterKeyword,            // after
    BeforeKeyword,           // before
    ReplaceKeyword,          // replace
    AtTopKeyword,            // at top
    AtBottomKeyword,         // at bottom
    FromKeyword,             // from
    AsKeyword,               // as
    ExceptKeyword,           // except
    UseKeyword,              // use
    
    // CHTL JS关键字
    VirKeyword,              // Vir
    ListenKeyword,           // Listen
    DelegateKeyword,         // Delegate
    AnimateKeyword,          // Animate
    RouterKeyword,           // Router
    ScriptLoaderKeyword,     // ScriptLoader
    
    // 增强选择器
    EnhancedSelector,        // {{selector}}
};

// Token类
class Token {
public:
    Token() : type(TokenType::Unknown) {}
    
    Token(TokenType t, const String& v, const Position& p)
        : type(t), value(v), position(p) {}
    
    Token(TokenType t, const String& v, const SourceRange& r)
        : type(t), value(v), position(r.start), range(r) {}
    
    TokenType getType() const { return type; }
    const String& getValue() const { return value; }
    const Position& getPosition() const { return position; }
    const SourceRange& getRange() const { return range; }
    
    bool is(TokenType t) const { return type == t; }
    bool isNot(TokenType t) const { return type != t; }
    bool isOneOf(TokenType t1, TokenType t2) const { return is(t1) || is(t2); }
    
    template<typename... Types>
    bool isOneOf(TokenType t1, TokenType t2, Types... rest) const {
        return is(t1) || isOneOf(t2, rest...);
    }
    
    bool isKeyword() const;
    bool isOperator() const;
    bool isLiteral() const;
    bool isComment() const;
    
    String toString() const;
    String typeToString() const;
    
private:
    TokenType type;
    String value;
    Position position;
    SourceRange range;
};

// Token类型转字符串
const char* tokenTypeToString(TokenType type);

} // namespace CHTL

#endif // CHTL_TOKEN_H
