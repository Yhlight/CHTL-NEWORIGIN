#pragma once

#include "Token.hpp"
#include <string>
#include <vector>

namespace chtl::core {

struct AstNode {
    std::string kind;
};

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    AstNode parse();

private:
    const std::vector<Token>& toks;
};

}
