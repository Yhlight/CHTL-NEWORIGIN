#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include <vector>
#include <memory>

namespace CHTL {

class CHTLParser {
public:
    explicit CHTLParser(std::vector<Token> tokens);

    // The main entry point for parsing. Returns the root of the AST.
    std::unique_ptr<ElementNode> parse();

private:
    // Recursive-descent parsing methods for different grammar rules.
    std::unique_ptr<BaseNode> parseNode();
    std::unique_ptr<ElementNode> parseElement();
    Attribute parseAttribute();
    std::unique_ptr<TextNode> parseTextNode();

    // Helper methods for token stream manipulation.
    const Token& currentToken() const;
    const Token& peekToken() const;
    void advance();
    bool match(TokenType type);
    const Token& consume(TokenType type, const std::string& errorMessage);

    std::vector<Token> m_tokens;
    size_t m_position = 0;
    Token m_eofToken;
};

} // namespace CHTL
