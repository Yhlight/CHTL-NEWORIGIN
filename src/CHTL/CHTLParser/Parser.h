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
        std::unique_ptr<BaseNode> parseStatement();
        std::unique_ptr<ElementNode> parseElement();
        std::unique_ptr<TextNode> parseTextNode();
        std::unique_ptr<StyleNode> parseStyleNode();
        std::unique_ptr<CommentNode> parseCommentNode();
        // A generic parseBlock method might be useful for element and text bodies
        // For now, let's keep it specific.

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
