#pragma once

#include "CHTLState.h"
#include <string>

class OriginState : public CHTLState {
public:
    OriginState();
    virtual void handle(CHTLParser& parser, Token token) override;
private:
    enum class Phase {
        EXPECT_AT,
        EXPECT_TYPE,
        EXPECT_NAME_OR_BRACE,
        EXPECT_BRACE,
        EXPECT_CONTENT
    };

    Phase phase;
    std::string type;
    std::string name;
};