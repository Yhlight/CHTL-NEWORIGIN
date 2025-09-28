#pragma once

#include "../Scanner/Tokenizer.h"
#include "../CHTLJS/CHTLJSCompiler.h" // Include compiler for recursion
#include <string>
#include <vector>
#include <map>

class CHTLJSPreprocessor {
public:
    // The process method now takes a compiler instance to allow for recursive compilation.
    void process(std::vector<Token>& tokens, CHTLJSCompiler& compiler);

private:
    // Maps Vir name -> (Function Signature -> vector of Tokens for the function body)
    std::map<std::string, std::map<std::string, std::vector<Token>>> ineveraway_map;
    int global_func_counter = 0;

    void findDeclarations(std::vector<Token>& tokens);
    void substituteAccesses(std::vector<Token>& tokens, CHTLJSCompiler& compiler);
};