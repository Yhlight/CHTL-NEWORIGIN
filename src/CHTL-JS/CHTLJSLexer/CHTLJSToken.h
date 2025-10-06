#ifndef CHTLJS_TOKEN_H
#define CHTLJS_TOKEN_H

#include "../../Common.h"

namespace CHTL {
namespace JS {

// CHTL JS Token类型
enum class JSTokenType {
    // 特殊类型
    EndOfFile,
    Unknown,
    
    // 字面量
    Identifier,
    StringLiteral,
    NumberLiteral,
    UnquotedLiteral,
    
    // 分隔符
    LeftBrace,          // {
    RightBrace,         // }
    LeftBracket,        // [
    RightBracket,       // ]
    LeftParen,          // (
    RightParen,         // )
    Semicolon,          // ;
    Colon,              // :
    Comma,              // ,
    Dot,                // .
    
    // 运算符
    Equal,              // =
    Plus,               // +
    Minus,              // -
    Star,               // *
    Slash,              // /
    Percent,            // %
    
    // 特殊运算符
    Arrow,              // ->
    EventBind,          // &->
    Ampersand,          // &
    Question,           // ?
    
    // CHTL JS关键字
    Vir,                // Vir
    Listen,             // Listen
    Delegate,           // Delegate
    Animate,            // Animate
    Router,             // Router
    ScriptLoader,       // ScriptLoader
    PrintMylove,        // PrintMylove
    INeverAway,         // INeverAway
    
    // 增强选择器
    EnhancedSelector,   // {{...}}
    
    // JavaScript关键字（基础）
    Const,
    Let,
    Var,
    Function,
    Return,
    If,
    Else,
    For,
    While,
    Break,
    Continue,
};

// CHTL JS Token类
class JSToken {
public:
    JSToken() : type(JSTokenType::Unknown) {}
    
    JSToken(JSTokenType t, const String& v, const Position& p)
        : type(t), value(v), position(p) {}
    
    JSTokenType getType() const { return type; }
    const String& getValue() const { return value; }
    const Position& getPosition() const { return position; }
    
    bool is(JSTokenType t) const { return type == t; }
    bool isNot(JSTokenType t) const { return type != t; }
    
    String toString() const;
    
private:
    JSTokenType type;
    String value;
    Position position;
};

const char* jsTokenTypeToString(JSTokenType type);

} // namespace JS
} // namespace CHTL

#endif // CHTLJS_TOKEN_H
