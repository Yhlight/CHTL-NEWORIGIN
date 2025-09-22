#pragma once

#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/CommentNode.h"
#include <memory>

namespace CHTL {

    class CHTLParser {
    public:
        CHTLParser(CHTLLexer& lexer);
        std::unique_ptr<BaseNode> parse();

    private:
        CHTLLexer& lexer;
        Token currentToken;

        void eat(TokenType type);
        void advance();

        std::unique_ptr<BaseNode> parseStatement();
        std::unique_ptr<TextNode> parseTextNode();
        std::unique_ptr<ElementNode> parseElementNode();
        std::unique_ptr<CommentNode> parseCommentNode();
    };

} // namespace CHTL
