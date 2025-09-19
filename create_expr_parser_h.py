import os

file_path = "CHTL/CHTLParser/ExpressionParser.h"
content = r"""
#pragma once

#include "../CHTLLexer/Token.h"
#include "Expr.h"
#include <vector>
#include <memory>

namespace CHTL {

// A parser that handles style property value expressions.
class ExpressionParser {
public:
    ExpressionParser(const std::vector<Token>& tokens);
    std::unique_ptr<Expr> parse();

private:
    const std::vector<Token>& tokens;
    size_t current = 0;

    bool isAtEnd();
    Token peek();
    Token previous();
    Token advance();
    bool check(TokenType type);
    bool match(const std::vector<TokenType>& types);

    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> ternary();
    std::unique_ptr<Expr> logicOr();
    std::unique_ptr<Expr> logicAnd();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();
};

} // namespace CHTL
"""

dir_name = os.path.dirname(file_path)
if not os.path.exists(dir_name):
    os.makedirs(dir_name)

with open(file_path, "w") as f:
    f.write(content.strip())

print(f"File '{file_path}' created successfully.")
