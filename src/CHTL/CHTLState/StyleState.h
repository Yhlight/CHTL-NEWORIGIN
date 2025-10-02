#pragma once

#include "CHTLState.h"

class StyleState : public CHTLState {
public:
    void handle(CHTLParser& parser, Token token) override;
};