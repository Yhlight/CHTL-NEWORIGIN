#pragma once

#include <string>
#include <vector>

namespace CHTL {

// Forward declarations
class CHTLContext;

// Token structure
struct Token {
    enum Type {
        IDENTIFIER,
        KEYWORD,
        OPERATOR,
        STRING_LITERAL,
        NUMBER,
        COMMENT,
        WHITESPACE,
        UNKNOWN
    };
    
    Type type;
    std::string value;
    int line;
    int column;
    
    Token(Type t, const std::string& v, int l = -1, int c = -1)
        : type(t), value(v), line(l), column(c) {}
};

// Lexer class - placeholder for ANTLR4 integration
class CHTLLexer {
public:
    CHTLLexer(CHTLContext& context);
    ~CHTLLexer();
    
    // Tokenization
    bool tokenize(const std::string& input);
    std::vector<Token> getTokens() const;
    void clearTokens();
    
private:
    CHTLContext& context_;
    std::vector<Token> tokens_;
};

} // namespace CHTL