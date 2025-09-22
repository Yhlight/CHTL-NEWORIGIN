#ifndef CHTL_EXPRESSION_PARSER_H
#define CHTL_EXPRESSION_PARSER_H

#include "ValueLexer.h"
#include "ExpressionAST.h"
#include <vector>
#include <memory>

class ExpressionParser {
public:
    explicit ExpressionParser(std::vector<ValueToken> tokens);
    std::unique_ptr<ExprNode> parse();

private:
    std::vector<ValueToken> tokens;
    size_t current = 0;

    std::unique_ptr<ExprNode> parse_expression();
    std::unique_ptr<ExprNode> parse_term();
    std::unique_ptr<ExprNode> parse_factor();
    std::unique_ptr<ExprNode> parse_primary();

    ValueToken& peek();
    ValueToken& advance();
    bool is_at_end();
};


#endif //CHTL_EXPRESSION_PARSER_H
