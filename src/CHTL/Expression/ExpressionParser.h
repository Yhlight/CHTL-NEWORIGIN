#ifndef EXPRESSION_PARSER_H
#define EXPRESSION_PARSER_H

#include "CHTL/CHTLLexer/Token.h"
#include "CHTL/Expression/ExpressionNode.h"
#include <vector>
#include <memory>

namespace CHTL {

class ExpressionParser {
public:
    ExpressionParser(const std::vector<Token>& tokens);
    std::unique_ptr<IExpressionNode> parse();

private:
    std::vector<Token> tokens;
    int current_token_index;

    Token peek();
    Token advance();

    // Recursive descent parsing methods
    std::unique_ptr<IExpressionNode> parseExpression();
    std::unique_ptr<IExpressionNode> parseTerm();
    std::unique_ptr<IExpressionNode> parseFactor();
    std::unique_ptr<IExpressionNode> parsePrimary();
};

} // namespace CHTL

#endif // EXPRESSION_PARSER_H
