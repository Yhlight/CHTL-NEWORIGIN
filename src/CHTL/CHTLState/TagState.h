#pragma once

#include "CHTLState.h"

class TagState : public CHTLState {
public:
    void handle(CHTLParser& parser, Token token) override;
};