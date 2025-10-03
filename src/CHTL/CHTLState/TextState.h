#pragma once

#include "CHTLState.h"

class TextState : public CHTLState {
public:
    void handle(CHTLParser& parser, Token token) override;
};