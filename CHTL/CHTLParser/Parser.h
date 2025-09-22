#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include <vector>
#include <memory>

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<ElementNode> parse(); // Returns a root node containing all top-level elements

private:
    std::vector<Token> tokens;
    int current = 0;

    std::unique_ptr<BaseNode> parseDeclaration();
    std::unique_ptr<ElementNode> parseElement();
    std::unique_ptr<TextNode> parseTextNode();
    void parseStyleBlock(ElementNode* parent);
    void parseAttribute(ElementNode* parent);
    void parseBlock(ElementNode* parent);

    bool isAtEnd();
    Token peek();
    Token peekNext();
    Token previous();
    Token advance();
    bool check(TokenType type);
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);
};
