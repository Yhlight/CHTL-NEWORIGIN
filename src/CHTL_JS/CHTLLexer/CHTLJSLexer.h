#ifndef CHTLJSLEXER_H
#define CHTLJSLEXER_H

#include <string>
#include <vector>
#include <map>

enum class CHTLJSTokenType {
    Identifier,
    String,
    Number,
    OpenBrace,      // {
    CloseBrace,     // }
    OpenParen,      // (
    CloseParen,     // )
    OpenBracket,    // [
    CloseBracket,   // ]
    OpenDoubleBrace, // {{
    CloseDoubleBrace, // }}
    Colon,          // :
    Semicolon,      // ;
    Comma,          // ,
    Dot,            // .
    RightArrow,     // ->
    EventBind,      // &->

    // Keywords
    Listen,
    Delegate,
    Animate,
    Router,
    Vir,
    ScriptLoader,
    Target,
    Load,

    // Raw JS block, to be treated as a single token by this lexer
    RawJavaScript,

    Unexpected,
    EndOfFile
};

struct CHTLJSToken {
    CHTLJSTokenType type;
    std::string value;
    int line;
    int column;
};

class CHTLJSLexer {
public:
    explicit CHTLJSLexer(const std::string& source);
    std::vector<CHTLJSToken> tokenize();

private:
    std::string source;
    size_t position;
    int line;
    int column;

    char peek();
    char advance();
    void skipWhitespace();
    CHTLJSToken identifier();
    CHTLJSToken stringLiteral(char quoteType);
    CHTLJSToken getNextToken();

    std::map<std::string, CHTLJSTokenType> keywords;
};

#endif // CHTLJSLEXER_H