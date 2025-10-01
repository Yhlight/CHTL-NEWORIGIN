#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include <vector>
#include <memory>
#include <string>
#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/ScriptNode.h"
#include "../CHTLNode/ExpressionNode.h"
#include "../CHTLNode/ConditionalExpressionNode.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLState/State.h"
#include <stack>

namespace CHTL {

class CHTLParser {
public:
    CHTLParser(const std::vector<Token>& tokens);
    void parse();
    std::vector<std::unique_ptr<BaseNode>> getAST();

    void setState(std::unique_ptr<State> newState);
    void addNodeToCurrent(std::unique_ptr<BaseNode> node);

    // Context stack management
    void pushNode(BaseNode* node);
    void popNode();
    BaseNode* currentNode() const;

    // Public token manipulation helpers for strategies
    Token peek() const;
    Token peekNext() const;
    Token advance();
    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool match(TokenType type);
    Token consume(TokenType type, const std::string& message);

    // Keep expression parsing public for strategies that need it.
    std::unique_ptr<ExpressionNode> parseExpression();

private:
    const std::vector<Token>& tokens;
    size_t current = 0;
    std::unique_ptr<State> currentState;
    std::stack<BaseNode*> nodeStack;
    std::vector<std::unique_ptr<BaseNode>> ast;


    // Expression parsing (Pratt/Precedence Climbing)
    std::unique_ptr<ExpressionNode> parseTernary();
    std::unique_ptr<ExpressionNode> parseLogicalOr();
    std::unique_ptr<ExpressionNode> parseLogicalAnd();
    std::unique_ptr<ExpressionNode> parseEquality();
    std::unique_ptr<ExpressionNode> parseComparison();
    std::unique_ptr<ExpressionNode> parseTerm();
    std::unique_ptr<ExpressionNode> parseFactor();
    std::unique_ptr<ExpressionNode> parsePower();
    std::unique_ptr<ExpressionNode> parsePrimary();
};

} // namespace CHTL

#endif // CHTL_PARSER_H