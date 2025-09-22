#pragma once

#include <string>
#include <vector>

enum class TokenType {
    // Special Tokens
    EndOfFile,
    Unexpected,

    // Literals
    Identifier,
    String,
    UnquotedLiteral,
    Number,

    // Punctuators
    OpenBrace,      // {
    CloseBrace,     // }
    OpenParen,      // (
    CloseParen,     // )
    OpenBracket,    // [
    CloseBracket,   // ]
    Colon,          // :
    Semicolon,      // ;
    Equals,         // =
    Comma,          // ,
    Dot,            // .

    // Operators
    Plus,           // +
    Minus,          // -
    Asterisk,       // *
    Slash,          // /
    Percent,        // %
    DoubleAsterisk, // **
    QuestionMark,   // ?
    Ampersand,      // &
    DoubleAmpersand,// &&
    DoublePipe,     // ||

    // Comments
    LineComment,      // //
    BlockComment,     // /* */
    GeneratorComment, // #

    // Keywords
    Text,
    Style,
    Script,

    // Keywords for Templates/Custom/etc.
    Template,         // [Template]
    Custom,           // [Custom]
    Origin,           // [Origin]
    Import,           // [Import]
    Namespace,        // [Namespace]
    Configuration,    // [Configuration]

    AtStyle,          // @Style
    AtElement,        // @Element
    AtVar,            // @Var
    AtHtml,           // @Html
    AtJavaScript,     // @JavaScript
    AtChtl,           // @Chtl
    AtConfig,         // @Config

    Inherit,
    Delete,
    Insert,
    After,
    Before,
    Replace,
    AtTop,
    AtBottom,
    From,
    As,
    Except,
    Use,
    Html5,

    // All other HTML tags will be treated as Identifiers initially
    // and resolved by the parser.
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

// Helper function for debugging
inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::EndOfFile: return "EndOfFile";
        case TokenType::Unexpected: return "Unexpected";
        case TokenType::Identifier: return "Identifier";
        case TokenType::String: return "String";
        case TokenType::UnquotedLiteral: return "UnquotedLiteral";
        case TokenType::Number: return "Number";
        case TokenType::OpenBrace: return "OpenBrace";
        case TokenType::CloseBrace: return "CloseBrace";
        case TokenType::OpenParen: return "OpenParen";
        case TokenType::CloseParen: return "CloseParen";
        case TokenType::OpenBracket: return "OpenBracket";
        case TokenType::CloseBracket: return "CloseBracket";
        case TokenType::Colon: return "Colon";
        case TokenType::Semicolon: return "Semicolon";
        case TokenType::Equals: return "Equals";
        case TokenType::Comma: return "Comma";
        case TokenType::Dot: return "Dot";
        case TokenType::Plus: return "Plus";
        case TokenType::Minus: return "Minus";
        case TokenType::Asterisk: return "Asterisk";
        case TokenType::Slash: return "Slash";
        case TokenType::Percent: return "Percent";
        case TokenType::DoubleAsterisk: return "DoubleAsterisk";
        case TokenType::QuestionMark: return "QuestionMark";
        case TokenType::Ampersand: return "Ampersand";
        case TokenType::DoubleAmpersand: return "DoubleAmpersand";
        case TokenType::DoublePipe: return "DoublePipe";
        case TokenType::LineComment: return "LineComment";
        case TokenType::BlockComment: return "BlockComment";
        case TokenType::GeneratorComment: return "GeneratorComment";
        case TokenType::Text: return "Text";
        case TokenType::Style: return "Style";
        case TokenType::Script: return "Script";
        case TokenType::Template: return "Template";
        case TokenType::Custom: return "Custom";
        case TokenType::Origin: return "Origin";
        case TokenType::Import: return "Import";
        case TokenType::Namespace: return "Namespace";
        case TokenType::Configuration: return "Configuration";
        case TokenType::AtStyle: return "AtStyle";
        case TokenType::AtElement: return "AtElement";
        case TokenType::AtVar: return "AtVar";
        case TokenType::AtHtml: return "AtHtml";
        case TokenType::AtJavaScript: return "AtJavaScript";
        case TokenType::AtChtl: return "AtChtl";
        case TokenType::AtConfig: return "AtConfig";
        case TokenType::Inherit: return "Inherit";
        case TokenType::Delete: return "Delete";
        case TokenType::Insert: return "Insert";
        case TokenType::After: return "After";
        case TokenType::Before: return "Before";
        case TokenType::Replace: return "Replace";
        case TokenType::AtTop: return "AtTop";
        case TokenType::AtBottom: return "AtBottom";
        case TokenType::From: return "From";
        case TokenType::As: return "As";
        case TokenType::Except: return "Except";
        case TokenType::Use: return "Use";
        case TokenType::Html5: return "Html5";
        default: return "Unknown";
    }
}
