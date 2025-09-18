#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/StyleNode.h"

#include <vector>
#include <memory>
#include <stdexcept>

namespace CHTL
{

    class Parser
    {
    public:
        explicit Parser(const std::vector<Token> &tokens);
        std::vector<std::unique_ptr<BaseNode>> parse();

    private:
        // Main parsing functions for different CHTL constructs
        // Statement-level parsing
        std::unique_ptr<BaseNode> parseStatement();
        std::unique_ptr<ElementNode> parseElement();
        std::unique_ptr<TextNode> parseTextNode();
        std::unique_ptr<StyleNode> parseStyleNode();
        std::unique_ptr<CommentNode> parseCommentNode();

        // Expression-level parsing (Pratt Parser)
        std::unique_ptr<ExprNode> parseExpression(int precedence = 0);
        std::unique_ptr<ExprNode> parsePrefix();
        std::unique_ptr<ExprNode> parseInfix(std::unique_ptr<ExprNode> left);
        int getPrecedence(TokenType type);


        // Token stream navigation and consumption helpers
        const Token &advance();
        const Token &peek() const;
        const Token &previous() const;
        bool isAtEnd() const;

        // Check the current token type without consuming it
        bool check(TokenType type) const;

        // If the current token has the given type, consume it and return true
        bool match(TokenType type);

        // Consume a specific token type, or throw an error if it's not found
        const Token &consume(TokenType type, const std::string &message);

        const std::vector<Token> &tokens;
        size_t current = 0;
    };

}
