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

    // The parse method starts the parsing process and is expected to return
    // the root element of the CHTL document.
    std::unique_ptr<ElementNode> parse();

private:
    void parseAttribute(ElementNode* node);
    std::unique_ptr<ElementNode> parseElement();
    std::unique_ptr<TextNode> parseTextStatement();

    // Helper methods for token handling
    void advance();
    bool match(TokenType type);
    void consume(TokenType type, const std::string& message);
    bool check(TokenType type);
    bool isAtEnd();
    const Token& peek();
    const Token& peekNext();
    const Token& previous();

    Lexer& lexer;
    Token currentToken;
    Token previousToken;
    Token lookaheadToken;
    bool hasLookahead = false;
    bool hadError = false;
};

#endif // CHTL_PARSER_H
