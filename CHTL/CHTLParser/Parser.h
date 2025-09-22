#pragma once

#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/LiteralNode.h"
#include "../CHTLNode/StyleNode.h"
#include <vector>
#include <string>
#include <memory>

class Parser {
public:
    Parser(Lexer& l);

    std::unique_ptr<Program> ParseProgram();
    const std::vector<std::string>& Errors() const { return errors; }

private:
    void nextToken();
    bool expectPeek(TokenType t);

    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<ElementNode> parseElementNode();
    std::unique_ptr<TextNode> parseTextNode();
    std::unique_ptr<Statement> parseAttributeNode();
    std::unique_ptr<Statement> parseStyleNode();
    std::unique_ptr<Expression> parseExpression();

    void peekError(TokenType t);

    Lexer& lexer;
    Token curToken;
    Token peekToken;
    std::vector<std::string> errors;
};
