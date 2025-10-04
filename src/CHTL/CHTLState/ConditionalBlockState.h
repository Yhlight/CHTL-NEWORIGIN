#pragma once

#include "CHTLState.h"
#include <string>

class ConditionalBlockState : public CHTLState {
public:
    ConditionalBlockState(const std::string& type);
    virtual void handle(CHTLParser& parser, Token token) override;

private:
    enum class Phase {
        EXPECT_KEYWORD_CONDITION,
        EXPECT_COLON,
        EXPECT_EXPRESSION,
        EXPECT_PROPERTY,
        EXPECT_VALUE
    };

    Phase phase;
    std::string conditionalType; // "if", "else if", "else"
    std::string condition;
    std::string currentProperty;
};