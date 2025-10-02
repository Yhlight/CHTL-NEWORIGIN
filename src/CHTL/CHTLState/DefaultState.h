#pragma once

#include "CHTLState.h"

class DefaultState : public CHTLState {
public:
    void handle(CHTLParser& parser, Token token) override;
};