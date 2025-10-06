#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/ExpressionNode.h"
#include <vector>
#include <memory>

namespace CHTL {
    class ExpressionParser {
    public:
        ExpressionParser(const std::vector<Token>& tokens);
        std::shared_ptr<ExpressionNode> parse();

    private:
        const std::vector<Token>& tokens;
        size_t position;

        std::shared_ptr<ExpressionNode> expression();
        std::shared_ptr<ExpressionNode> logical_or();
        std::shared_ptr<ExpressionNode> logical_and();
        std::shared_ptr<ExpressionNode> equality();
        std::shared_ptr<ExpressionNode> comparison();
        std::shared_ptr<ExpressionNode> term();
        std::shared_ptr<ExpressionNode> factor();
        std::shared_ptr<ExpressionNode> unary();
        std::shared_ptr<ExpressionNode> primary();

        bool isAtEnd();
        Token peek();
        Token previous();
        Token advance();
        bool check(TokenType type);
        bool match(const std::vector<TokenType>& types);
    };
}