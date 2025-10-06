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

    Token& getCurrentToken();
    void advance();
    bool isAtEnd();

private:
    std::vector<Token> tokens;
    size_t position;

    std::shared_ptr<BaseNode> parseSelector();
};

}