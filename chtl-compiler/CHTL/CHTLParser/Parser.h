#pragma once

#include "chtl-compiler/CHTL/CHTLLexer/Lexer.h"
#include "chtl-compiler/CHTL/CHTLNode/Node.h"
#include "chtl-compiler/CHTL/CHTLNode/ElementNode.h"
#include "chtl-compiler/CHTL/CHTLNode/TextNode.h"
#include "chtl-compiler/CHTL/CHTLNode/CommentNode.h"
#include "chtl-compiler/CHTL/CHTLTemplate/TemplateRegistry.h"
#include "chtl-compiler/CHTL/Expression/ExpressionParser.h"
#include "chtl-compiler/CHTL/Loader/Loader.h"
#include <memory>

class Parser {
public:
    Parser(Lexer& lexer, Loader& loader, const std::string& currentFile);
    NodeList parse();

private:
    Lexer& lexer;
    Loader& loader;
    std::string currentFile;
    Token currentToken;
    Token peekToken;

    void eat(TokenType type);
    void consume(); // A helper to advance both current and peek tokens
    NodePtr parseElement();
    void parseAttributes(std::shared_ptr<ElementNode> element);
    void parseStyleBlock(std::shared_ptr<ElementNode> element);
    NodePtr parseTemplateDefinition();
    NodeList parseElementUsage();
    void parseImportStatement();
    NodePtr parseText();
    NodePtr parseComment();
    NodeList parseStatement();
};