#pragma once

#include "TokenTypes.h" // Changed from Common/TokenTypes.h
#include <string>
#include <vector>

struct Token {
    TokenType type;
    std::string lexeme;
    int line = 0;
    int col = 0;
};
