#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/Nodes.h"
#include <vector>
#include <memory>

namespace CHTL {

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::vector<NodePtr> parse();

private:
    NodePtr parseDeclaration();
    NodePtr parseStatement();
    NodePtr parseElement();
    NodePtr parseText();
    NodePtr parseImport();
    void parseAttributes(ElementNode& element);
    std::shared_ptr<StyleBlockNode> parseStyleBlock();

    // Helper methods
    Token advance();
    Token peek();
    Token peekNext();
    Token previous();
    bool isAtEnd();
    bool check(TokenType type);
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);
    void synchronize(); // For error recovery

    const std::vector<Token>& tokens;
    int current = 0;
};

} // namespace CHTL
