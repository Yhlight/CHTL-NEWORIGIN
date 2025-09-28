#pragma once

#include <string>
#include <vector>

// Defines the token types for the CHTL JS language.
enum class CHTLJSTokenType {
    // CHTL JS specific syntax
    OpenDoubleBrace,  // {{
    CloseDoubleBrace, // }}
    Arrow,            // ->
    EventBindingOperator, // &->

    // Keywords
    ScriptLoader,     // ScriptLoader keyword
    Vir,              // Vir keyword for virtual objects
    Router,           // Router keyword
    Placeholder,      // A block of raw JavaScript code
    ReservedPlaceholder, // __CHTL_RESERVED_PLACEHOLDER__{...}__

    // General tokens
    Identifier,       // e.g., click, custom function names
    StringLiteral,    // For file paths in ScriptLoader

    // Punctuation
    OpenBrace,        // {
    CloseBrace,       // }
    OpenBracket,      // [
    CloseBracket,     // ]
    Colon,            // :
    Comma,            // ,
    Semicolon,        // ;
    Equals,           // =

    // Special tokens
    EndOfFile,
    Unexpected,
};

// Represents a single token extracted from the CHTL JS source.
struct CHTLJSToken {
    CHTLJSTokenType type;
    std::string value;
};

// The CHTL JS Lexer, responsible for tokenizing CHTL JS source code.
class CHTLJSLexer {
public:
    explicit CHTLJSLexer(const std::string& source);
    std::vector<CHTLJSToken> tokenize();

private:
    std::string source;
    size_t position = 0;

    CHTLJSToken getNextToken();
    char peek(int offset = 0) const;
    char advance();
};