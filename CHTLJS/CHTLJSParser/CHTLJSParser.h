#pragma once

#include "../CHTLJSLexer/Token.h"
#include "../CHTLJSNode/BaseNode.h"
#include <vector>
#include <memory>

namespace CHTLJS {

    class CHTLJSParser {
    public:
        CHTLJSParser(const std::vector<Token>& tokens);
        std::shared_ptr<BaseNode> parse();
        bool isAtEnd();
        Token& getCurrentToken();
        Token advance();

    private:
        std::vector<Token> tokens;
        size_t position;

        Token peek(size_t offset = 0);
        Token previous();
        bool check(TokenType type);
        bool match(const std::vector<TokenType>& types);
    };

}