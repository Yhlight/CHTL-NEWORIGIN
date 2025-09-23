#pragma once

#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include <memory>
#include <vector>

// The Parser class consumes tokens from the Lexer to build an
// Abstract Syntax Tree (AST) representing the CHTL document structure.
class Parser {
public:
    explicit Parser(Lexer& lexer);

    // The main entry point for parsing. It returns a vector of root nodes,
    // as a CHTL file can contain multiple elements at the top level.
    std::vector<std::unique_ptr<BaseNode>> parse();

private:
    Lexer& lexer;
    Token currentToken;
    Token peekToken;

    // Advances the token stream.
    void advanceTokens();

    // Consumes the current token if it matches the expected type,
    // otherwise throws an error.
    void expectToken(TokenType type);

    // Parsing methods for different CHTL constructs.
    std::unique_ptr<BaseNode> parseStatement();
    std::unique_ptr<ElementNode> parseElement();
    std::unique_ptr<TextNode> parseTextElement();
    std::unique_ptr<BaseNode> parseComment();
    void parseAttribute(ElementNode* element);
    void parseStyleBlock(ElementNode* element);
};
