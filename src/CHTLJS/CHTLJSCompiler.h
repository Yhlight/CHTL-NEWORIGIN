#pragma once

#include "Scanner/Tokenizer.h" // Include the Token definition
#include <string>
#include <vector>
#include <map>

class CHTLJSCompiler {
public:
    // The main compile method now takes a vector of tokens.
    std::string compile(std::vector<Token>& tokens);

private:
    // Helper methods will also be updated to work with tokens,
    // but their signatures can remain private for now.
    // We will refactor them in the .cpp file.
};