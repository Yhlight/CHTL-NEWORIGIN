#pragma once

#include "CHTLState.h"

class ScriptState : public CHTLState {
public:
    void handle(CHTLParser& parser, Token token) override;
};