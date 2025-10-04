#pragma once

#include "CHTLState.h"
#include <string>

class DefinitionState : public CHTLState {
public:
    DefinitionState(const std::string& category);
    void handle(CHTLParser& parser, Token token) override;

private:
    std::string category;
};