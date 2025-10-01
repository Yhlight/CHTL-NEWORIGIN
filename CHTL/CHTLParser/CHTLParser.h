#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include <vector>
#include <memory>
#include <string>
#include <stack>
#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"

namespace CHTL {

class ParsingState; // Forward-declaration

class CHTLParser {
public:
    CHTLParser(const std::vector<Token>& tokens);
    ~CHTLParser(); // Declare the destructor
    std::unique_ptr<BaseNode> parse();

    // State management
    void setState(std::unique_ptr<ParsingState> newState);

    // Token manipulation helpers (for states)
    Token peek() const;
    Token peekNext() const;
    Token advance();
    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool match(TokenType type);
    Token consume(TokenType type, const std::string& message);

    // AST construction helpers (for states)
    ElementNode* getCurrentNode();
    void openNode(std::unique_ptr<ElementNode> node);
    void closeNode();
    void addNode(std::unique_ptr<BaseNode> node);

    // Expression parsing helpers (for strategies)
    std::unique_ptr<ExpressionNode> parseExpression();

private:
    std::unique_ptr<ExpressionNode> parseTerm();
    std::unique_ptr<ExpressionNode> parseFactor();
    std::unique_ptr<ExpressionNode> parsePower();
    std::unique_ptr<ExpressionNode> parsePrimary();
    const std::vector<Token>& tokens;
    size_t current = 0;
    std::unique_ptr<ParsingState> state;
    std::unique_ptr<ElementNode> root;
    std::stack<ElementNode*> nodeStack;
};

} // namespace CHTL

#endif // CHTL_PARSER_H