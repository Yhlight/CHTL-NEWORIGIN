#pragma once

#include "CHTLState.h"
#include <string>

class TagState : public CHTLState {
public:
    TagState();
    void handle(CHTLParser& parser, Token token) override;
private:
    std::string pendingAttributeName;
    bool expectingValue;
};