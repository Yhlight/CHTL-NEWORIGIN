#pragma once

#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/Node.h"
#include "../CHTLNode/ElementNode.h" // Include the ElementNode definition
#include <vector>
#include <string>

class Parser {
public:
    explicit Parser(Lexer& lexer);

    // The main entry point for parsing. Returns the root node of the AST.
    // For now, we assume a CHTL file consists of a single root element.
    NodePtr parseProgram();

    // Returns a list of any parsing errors encountered.
    const std::vector<std::string>& errors() const { return m_errors; }

private:
    // Helper to advance to the next token.
    void nextToken();

    // Asserts the type of the next token and advances if it matches.
    // If it doesn't match, it records an error.
    bool expectPeek(TokenType t);

    // Parsing functions for different grammar rules.
    NodePtr parseStatement();
    NodePtr parseElementStatement();
    NodePtr parseTextStatement();
    void parseAttribute(std::shared_ptr<ElementNode> node);

    Lexer& m_lexer;
    Token m_curToken;
    Token m_peekToken;

    std::vector<std::string> m_errors;
};
