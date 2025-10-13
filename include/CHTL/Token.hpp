#pragma once

#include "../Common.hpp"

namespace CHTL {

// Token类型枚举
enum class TokenType {
    // 标识符和字面量
    Identifier,         // 标识符
    String,            // 字符串字面量
    Number,            // 数字字面量
    UnquotedLiteral,   // 无修饰字面量
    
    // 关键字
    Text,              // text
    Style,             // style
    Script,            // script
    Template,          // [Template]
    Custom,            // [Custom]
    Origin,            // [Origin]
    Import,            // [Import]
    Namespace,         // [Namespace]
    Configuration,     // [Configuration]
    Info,              // [Info]
    Export,            // [Export]
    
    // 模板/自定义类型
    AtStyle,           // @Style
    AtElement,         // @Element
    AtVar,             // @Var
    AtHtml,            // @Html
    AtJavaScript,      // @JavaScript
    AtChtl,            // @Chtl
    AtCJmod,           // @CJmod
    AtConfig,          // @Config
    
    // 操作符
    Colon,             // :
    Semicolon,         // ;
    Comma,             // ,
    Dot,               // .
    Equal,             // =
    Plus,              // +
    Minus,             // -
    Multiply,          // *
    Divide,            // /
    Modulo,            // %
    Power,             // **
    LessThan,          // <
    GreaterThan,       // >
    LessEqual,         // <=
    GreaterEqual,      // >=
    EqualEqual,        // ==
    NotEqual,          // !=
    And,               // &&
    Or,                // ||
    Question,          // ?
    Ampersand,         // &
    Arrow,             // ->
    EventBind,         // &->
    
    // 分隔符
    LeftBrace,         // {
    RightBrace,        // }
    LeftBracket,       // [
    RightBracket,      // ]
    LeftParen,         // (
    RightParen,        // )
    
    // 关键字
    From,              // from
    As,                // as
    Use,               // use
    Inherit,           // inherit
    Delete,            // delete
    Insert,            // insert
    After,             // after
    Before,            // before
    Replace,           // replace
    AtTop,             // at top
    AtBottom,          // at bottom
    Except,            // except
    If,                // if
    Else,              // else
    ElseIf,            // else if
    
    // 特殊
    Comment,           // 注释 // 或 /**/
    GeneratorComment,  // 生成器注释 #
    DoubleLeftBrace,   // {{
    DoubleRightBrace,  // }}
    DollarSign,        // $
    
    // 辅助类型
    EndOfFile,         // 文件结束
    Unknown            // 未知
};

class Token {
public:
    Token(TokenType type, String value, int line, int column)
        : type_(type), value_(std::move(value)), line_(line), column_(column) {}
    
    TokenType type() const { return type_; }
    const String& value() const { return value_; }
    int line() const { return line_; }
    int column() const { return column_; }
    
    bool is(TokenType type) const { return type_ == type; }
    bool isNot(TokenType type) const { return type_ != type; }
    
    String toString() const {
        return "Token(" + typeToString(type_) + ", \"" + value_ + "\", " + 
               std::to_string(line_) + ":" + std::to_string(column_) + ")";
    }
    
    static String typeToString(TokenType type) {
        switch (type) {
            case TokenType::Identifier: return "Identifier";
            case TokenType::String: return "String";
            case TokenType::Number: return "Number";
            case TokenType::UnquotedLiteral: return "UnquotedLiteral";
            case TokenType::Text: return "Text";
            case TokenType::Style: return "Style";
            case TokenType::Script: return "Script";
            case TokenType::Template: return "Template";
            case TokenType::Custom: return "Custom";
            case TokenType::Origin: return "Origin";
            case TokenType::Import: return "Import";
            case TokenType::Namespace: return "Namespace";
            case TokenType::Configuration: return "Configuration";
            case TokenType::Info: return "Info";
            case TokenType::Export: return "Export";
            case TokenType::AtStyle: return "AtStyle";
            case TokenType::AtElement: return "AtElement";
            case TokenType::AtVar: return "AtVar";
            case TokenType::AtHtml: return "AtHtml";
            case TokenType::AtJavaScript: return "AtJavaScript";
            case TokenType::AtChtl: return "AtChtl";
            case TokenType::AtCJmod: return "AtCJmod";
            case TokenType::AtConfig: return "AtConfig";
            case TokenType::LeftBrace: return "LeftBrace";
            case TokenType::RightBrace: return "RightBrace";
            case TokenType::LeftBracket: return "LeftBracket";
            case TokenType::RightBracket: return "RightBracket";
            case TokenType::LeftParen: return "LeftParen";
            case TokenType::RightParen: return "RightParen";
            case TokenType::Colon: return "Colon";
            case TokenType::Semicolon: return "Semicolon";
            case TokenType::Comma: return "Comma";
            case TokenType::Dot: return "Dot";
            case TokenType::Equal: return "Equal";
            case TokenType::Plus: return "Plus";
            case TokenType::Minus: return "Minus";
            case TokenType::Multiply: return "Multiply";
            case TokenType::Divide: return "Divide";
            case TokenType::Modulo: return "Modulo";
            case TokenType::Power: return "Power";
            case TokenType::Comment: return "Comment";
            case TokenType::GeneratorComment: return "GeneratorComment";
            case TokenType::From: return "From";
            case TokenType::As: return "As";
            case TokenType::Use: return "Use";
            case TokenType::Inherit: return "Inherit";
            case TokenType::Delete: return "Delete";
            case TokenType::Insert: return "Insert";
            case TokenType::After: return "After";
            case TokenType::Before: return "Before";
            case TokenType::Replace: return "Replace";
            case TokenType::AtTop: return "AtTop";
            case TokenType::AtBottom: return "AtBottom";
            case TokenType::Except: return "Except";
            case TokenType::If: return "If";
            case TokenType::Else: return "Else";
            case TokenType::ElseIf: return "ElseIf";
            case TokenType::EndOfFile: return "EndOfFile";
            default: return "Unknown";
        }
    }
    
private:
    TokenType type_;
    String value_;
    int line_;
    int column_;
};

} // namespace CHTL
