#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include <vector>
#include <memory>

#include "../CHTLLexer/Token.h" // For Token and TokenType
#include <stdexcept> // For std::runtime_error

// Forward declarations
class CHTLNode;

class CHTLParser {
public:
    // Public error class for catching
    class ParseError : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    explicit CHTLParser(const std::vector<Token>& tokens);
    std::unique_ptr<CHTLNode> parse();

private:
    // Recursive descent parsing methods
    std::unique_ptr<CHTLNode> declaration();
    std::unique_ptr<CHTLNode> elementDeclaration();
    std::unique_ptr<CHTLNode> textDeclaration();
    std::vector<std::unique_ptr<CHTLNode>> block();

    // Token handling utilities
    bool isAtEnd();
    Token advance();
    Token peek();
    Token previous();
    bool check(TokenType type);
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);

    const std::vector<Token>& tokens;
    size_t current = 0;

    // Error handling
    ParseError error(const Token& token, const std::string& message);
};

#endif // CHTL_PARSER_H
