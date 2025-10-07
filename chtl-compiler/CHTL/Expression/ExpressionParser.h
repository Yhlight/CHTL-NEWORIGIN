#pragma once

#include "../CHTLLexer/Token.h"
#include "ExprNode.h"
#include "LiteralExprNode.h"
#include "BinaryOpExprNode.h"
#include "ConditionalExprNode.h"
#include "PropertyAccessNode.h"
#include <vector>
#include <map>

class ExpressionParser {
public:
    ExpressionParser(const std::vector<Token>& tokens);
    ExprNodePtr parse();

private:
    std::vector<Token> tokens;
    size_t pos = 0;

    Token current_token();
    void consume_token();

    int get_token_precedence();
    ExprNodePtr parse_primary();
    ExprNodePtr parse_binary_op_rhs(int expr_prec, ExprNodePtr lhs);
    ExprNodePtr parse_conditional(ExprNodePtr condition);
};