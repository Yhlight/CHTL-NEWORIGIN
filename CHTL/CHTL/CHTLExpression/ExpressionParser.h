#pragma once

#include "../CHTLNode/ExpressionNode.h"
#include "../CHTLLexer/Token.h"
#include <vector>
#include <memory>

namespace CHTL {

class ExpressionParser {
public:
    ExpressionParser(const std::vector<Token>& tokens);
    std::unique_ptr<ExpressionNode> parse();

private:
    std::unique_ptr<ExpressionNode> addition();
    std::unique_ptr<ExpressionNode> multiplication();
    std::unique_ptr<ExpressionNode> primary();

    bool isAtEnd();
    Token advance();
    Token peek();
    bool check(TokenType type);
    bool match(const std::vector<TokenType>& types);

    const std::vector<Token>& tokens;
    int current = 0;
};

} // namespace CHTL
