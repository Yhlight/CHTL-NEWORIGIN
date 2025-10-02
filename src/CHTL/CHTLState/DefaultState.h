#pragma once

#include "CHTLState.h"
#include <string>

class DefaultState : public CHTLState {
public:
    void handle(CHTLParser& parser, Token token) override;
private:
    std::string pendingTagName;
};