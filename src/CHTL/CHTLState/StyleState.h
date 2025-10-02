#pragma once

#include "CHTLState.h"
#include <string>

class StyleState : public CHTLState {
public:
    StyleState();
    void handle(CHTLParser& parser, Token token) override;
private:
    std::string pendingPropertyName;
    bool expectingValue;
};