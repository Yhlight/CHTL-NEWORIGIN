#pragma once

#include "Expression.h"
#include "../CHTLLexer/Token.h"
#include <vector>
#include <memory>

/*
 * This is a simple recursive descent parser for style value expressions.
 * It handles basic arithmetic.
 */
class ExpressionParser {
public:
    explicit ExpressionParser(const std::vector<Token>& tokens);
    std::unique_ptr<Expr> parse();

private:
    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> addition();
    std::unique_ptr<Expr> primary();

    bool isAtEnd();
    Token peek();
    Token previous();
    Token advance();
    bool check(TokenType type);
    bool match(const std::vector<TokenType>& types);

    const std::vector<Token>& tokens;
    int current = 0;
};
