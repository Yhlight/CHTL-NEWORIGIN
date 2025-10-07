#pragma once

#include "chtl-compiler/CHTL/CHTLLexer/Lexer.h"
#include "chtl-compiler/CHTL/CHTLNode/Node.h"
#include "chtl-compiler/CHTL/CHTLNode/ElementNode.h"
#include "chtl-compiler/CHTL/CHTLNode/TextNode.h"
#include <memory>

class Parser {
public:
    Parser(Lexer& lexer);
    NodeList parse();

private:
    Lexer& lexer;
    Token currentToken;

    void eat(TokenType type);
    NodePtr parseElement();
    NodePtr parseText();
    NodePtr parseStatement();
};