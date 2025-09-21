#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/ProgramNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include <vector>
#include <memory>
#include <stdexcept>

namespace CHTL {

class Parser {
public:
    // Takes a vector of tokens from the lexer.
    explicit Parser(const std::vector<Token>& tokens);

    // The main entry point for parsing. Returns the root of the AST.
    std::unique_ptr<ProgramNode> parse();

private:
    // Main parsing methods for different language constructs.
    std::unique_ptr<BaseNode> parseStatement();
    std::unique_ptr<ElementNode> parseElement();
    std::unique_ptr<TextNode> parseTextNode();
    void parseAttributes(ElementNode& element);

    // Utility methods for token manipulation and error handling.
    void consume(TokenType type, const std::string& message);
    bool match(TokenType type);
    bool check(TokenType type) const;
    const Token& advance();
    const Token& peek() const;
    bool isAtEnd() const;

    // Custom exception for parsing errors.
    class ParseError : public std::runtime_error {
    public:
        explicit ParseError(const std::string& message) : std::runtime_error(message) {}
    };

    const std::vector<Token>& tokens;
    size_t current = 0;
};

} // namespace CHTL
