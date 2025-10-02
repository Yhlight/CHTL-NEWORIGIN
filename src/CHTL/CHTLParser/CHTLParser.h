#pragma once

#include <memory>
#include <string>
#include "../CHTLState/CHTLState.h"
#include "../CHTLLexer/CHTLLexer.h"

class CHTLParser {
public:
    CHTLParser(const std::string& input);
    void parse();
    void setState(std::unique_ptr<CHTLState> newState);
    Token getNextToken();

private:
    std::unique_ptr<CHTLState> currentState;
    CHTLLexer lexer;
};