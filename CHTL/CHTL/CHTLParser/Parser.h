#pragma once

#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/Node.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/CommentNode.h"
#include <vector>
#include <string>

class Parser {
public:
    explicit Parser(Lexer& lexer);

    NodePtr parseProgram();

    const std::vector<std::string>& errors() const { return m_errors; }

private:
    void nextToken();
    bool expectPeek(TokenType t);

    NodePtr parseStatement();
    NodePtr parseElementStatement();
    NodePtr parseTextStatement();
    NodePtr parseCommentStatement();
    void parseAttribute(std::shared_ptr<ElementNode> node);
    void parseStyleStatement(std::shared_ptr<ElementNode> node);

    Lexer& m_lexer;
    Token m_curToken;
    Token m_peekToken;

    std::vector<std::string> m_errors;
};
