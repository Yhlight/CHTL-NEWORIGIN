#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include "TokenType.h"
#include <string>

namespace CHTL
{
    struct Token
    {
        TokenType type;
        std::string value;
        size_t line;
        size_t column;
    };
}

#endif // CHTL_TOKEN_H
