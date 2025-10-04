#pragma once

#include "CHTLState.h"
#include <string>

class DefinitionState : public CHTLState {
public:
    DefinitionState(const std::string& category);
    void handle(CHTLParser& parser, Token token) override;

private:
    enum class Phase {
        EXPECT_TYPE,
        EXPECT_NAME,
        EXPECT_BRACE,
        INSIDE_BLOCK
    };

    Phase phase;
    std::string category;
    std::string type;
    std::string name;
    std::string pendingPropertyName;
    bool expectingValue = false;
};