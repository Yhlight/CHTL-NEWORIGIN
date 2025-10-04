#pragma once

#include "CHTLState.h"
#include <string>

class NamespaceState : public CHTLState {
public:
    void handle(CHTLParser& parser, Token token) override;
};