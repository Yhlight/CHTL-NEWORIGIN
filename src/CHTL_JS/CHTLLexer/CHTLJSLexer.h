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

    // General tokens
    Identifier,       // e.g., Listen, Delegate, click
    RawJS,            // A block of plain JavaScript code

    // Punctuation
    OpenBrace,        // {
    CloseBrace,       // }
    Colon,            // :
    Comma,            // ,

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

    // Helper functions for token creation
    CHTLJSToken makeToken(CHTLJSTokenType type, const std::string& value);
    CHTLJSToken makeToken(CHTLJSTokenType type, char value);
};