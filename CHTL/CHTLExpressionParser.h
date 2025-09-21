#pragma once

#include "CHTLNode/Nodes.h"
#include "CHTLLexer/Token.h"
#include <vector>
#include <memory>

namespace CHTL {

class ExpressionParser {
public:
    ExpressionParser(const std::vector<Token>& tokens);
    std::shared_ptr<ExprNode> parse();

private:
    // Pratt parser implementation details will go here
    std::shared_ptr<ExprNode> parsePrecedence(int precedence);
    std::shared_ptr<ExprNode> prefixParse();
    std::shared_ptr<ExprNode> infixParse(std::shared_ptr<ExprNode> left);
    int getPrecedence();

    const std::vector<Token>& tokens;
    size_t current = 0;

    Token advance();
    Token peek();
    bool isAtEnd();
};

} // namespace CHTL
