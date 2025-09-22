#pragma once

#include "TokenTypes.h" // Changed from Common/TokenTypes.h
#include <string>
#include <vector>
#include <map>

struct CHTLConfiguration {
    int index_initial_count = 0;
    bool debug_mode = false;
};

class CHTLContext {
public:
    CHTLContext();

    CHTLConfiguration config;

    std::map<std::string, TokenType> keyword_to_token_map;

private:
    void initialize_default_keywords();
};
