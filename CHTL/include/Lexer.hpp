#pragma once

#include "Token.hpp"
#include <string>
#include <vector>

namespace chtl::core {

class Lexer {
public:
    explicit Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    const std::string& src;
};

}
