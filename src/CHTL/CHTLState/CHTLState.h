#pragma once

#include "../CHTLLexer/Token.h"

// Forward declaration to avoid circular dependencies
class CHTLParser;

class CHTLState {
public:
    virtual ~CHTLState() = default;

    // Pure virtual function to be implemented by concrete state classes
    virtual void handle(CHTLParser& parser, Token token) = 0;
};