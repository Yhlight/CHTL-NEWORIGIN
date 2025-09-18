#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include <vector>
#include <memory>
#include <stdexcept>

namespace CHTL {

// Forward-declare node types used in parser methods
class ElementNode;

class Parser {
public:
    Parser(std::vector<Token> tokens);
    NodePtr parse();

private:
    std::vector<Token> tokens;
    size_t current = 0;

    bool isAtEnd();
    Token peek();
    Token peekNext();
    Token previous();
    Token advance();
    bool check(TokenType type);
    bool match(const std::vector<TokenType>& types);

    // Parsing methods for different grammar rules
    NodePtr declaration();
    NodePtr element();
    NodePtr textNode();
    void parseAttribute(ElementNode& owner);

    void synchronize(); // For error recovery
    Token consume(TokenType type, const std::string& message);
    class ParseError : public std::runtime_error {
    public:
        ParseError(const std::string& msg) : std::runtime_error(msg) {}
    };
    ParseError error(const Token& token, const std::string& message);
};

} // namespace CHTL
