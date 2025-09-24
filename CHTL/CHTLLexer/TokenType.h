#ifndef CHTL_TOKEN_TYPE_H
#define CHTL_TOKEN_TYPE_H

namespace CHTL
{
    enum class TokenType
    {
        // Special Tokens
        EndOfFile,
        Unexpected,

        // Literals
        Identifier,
        StringLiteral,
        UnquotedLiteral,
        NumberLiteral,

        // Punctuation
        OpenBrace,      // {
        CloseBrace,     // }
        OpenParen,      // (
        CloseParen,     // )
        OpenBracket,    // [
        CloseBracket,   // ]
        Semicolon,      // ;
        Colon,          // :
        Equals,         // =
        Comma,          // ,
        Dot,            // .
        At,             // @

        // Operators
        Plus,           // +
        Minus,          // -
        Asterisk,       // *
        Slash,          // /
        Percent,        // %
        DoubleAsterisk, // **
        GreaterThan,    // >
        LessThan,       // <
        QuestionMark,   // ?
        Ampersand,      // &
        DoubleAmpersand,// &&
        DoublePipe,     // ||

        // Comments
        LineComment,        // //
        BlockComment,       // /* ... */
        GeneratorComment,   // #

        // Keywords (will be handled as identifiers initially, but good to have)
        // We can use a map to distinguish keywords from identifiers
        KeywordText,
        KeywordStyle,
        KeywordScript,
        KeywordTemplate,
        KeywordCustom,
        KeywordOrigin,
        KeywordImport,
        KeywordNamespace,
        KeywordConfiguration,
        KeywordUse,
        KeywordFrom,
        KeywordAs,
        KeywordInherit,
        KeywordDelete,
        KeywordInsert,
        KeywordAfter,
        KeywordBefore,
        KeywordReplace,
        KeywordAtTop,
        KeywordAtBottom,
        KeywordExcept,
    };
}

#endif // CHTL_TOKEN_TYPE_H
