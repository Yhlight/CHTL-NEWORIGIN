#pragma once

#include "Token.h"
#include <string>

namespace CHTLJS {

class CHTLJSLexer {
public:
    CHTLJSLexer(const std::string& source);
    Token getNextToken();

private:
    std::string source;
    size_t position;

    char peek();
    char advance();
    void skipWhitespace();
};

}