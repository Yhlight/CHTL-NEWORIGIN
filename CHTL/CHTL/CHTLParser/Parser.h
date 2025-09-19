#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include <vector>
#include <memory>
#include <stdexcept>

class Parser {
public:
    Parser(Lexer& lexer);

    // In CHTL, a file can contain multiple root-level elements.
    // So the parser will return a list of root nodes.
    // For simplicity in this initial step, we'll parse a single root element.
    std::unique_ptr<BaseNode> parse();

private:
    std::unique_ptr<BaseNode> parseStatement();
    std::unique_ptr<ElementNode> parseElement();
    std::unique_ptr<TextNode> parseTextStatement();

    // Helper methods for token handling
    void advance();
    bool match(TokenType type);
    void consume(TokenType type, const std::string& message);
    bool check(TokenType type);
    bool isAtEnd();
    const Token& peek();
    const Token& previous();

    Lexer& lexer;
    Token currentToken;
    Token previousToken;
    bool hadError = false;
};

#endif // CHTL_PARSER_H
