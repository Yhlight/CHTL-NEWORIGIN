#pragma once

#include "CHTLState.h"

class ImportState : public CHTLState {
public:
    void handle(CHTLParser& parser, Token token) override;
};