#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include <vector>
#include <memory>

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    std::unique_ptr<BaseNode> parse();

private:
    std::vector<Token> tokens;
    size_t current = 0;
};