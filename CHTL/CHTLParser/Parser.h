#pragma once

#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/Nodes.h"
#include <memory>
#include <vector>
#include <string>

namespace CHTL {

class Parser {
public:
    explicit Parser(Lexer& lexer);

    std::unique_ptr<ElementNode> ParseProgram();
    const std::vector<std::string>& Errors() const;

private:
    void nextToken();

    std::unique_ptr<BaseNode> parseStatement();
    std::unique_ptr<ElementNode> parseElementNode();
    std::unique_ptr<AttributeNode> parseAttributeNode();
    std::unique_ptr<TextNode> parseTextNode();
    std::unique_ptr<TextNode> parseTextProperty();
    std::unique_ptr<StyleNode> parseStyleNode();

    // Helper methods for checking tokens
    bool currentTokenIs(TokenType type) const;
    bool peekTokenIs(TokenType type) const;
    bool expectPeek(TokenType type);
    void peekError(TokenType type);

    Lexer& m_lexer;
    Token m_currentToken;
    Token m_peekToken;

    std::vector<std::string> m_errors;
};

} // CHTL
