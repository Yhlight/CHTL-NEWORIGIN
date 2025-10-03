#pragma once

#include "CHTLState.h"
#include <string>

class ScriptState : public CHTLState {
public:
    ScriptState();
    void handle(CHTLParser& parser, Token token) override;
};