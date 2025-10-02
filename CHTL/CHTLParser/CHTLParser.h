#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include <vector>
#include <memory>

namespace CHTL {

    class CHTLParser {
    public:
        CHTLParser(const std::vector<Token>& tokens);
        std::shared_ptr<BaseNode> parse();

    private:
        std::vector<Token> tokens;
        size_t position;

        Token peek();
        Token advance();
        bool isAtEnd();
        std::shared_ptr<ElementNode> parseElement();
        std::shared_ptr<TextNode> parseText();
        void parseAttributes(std::shared_ptr<ElementNode> element);
    };

}