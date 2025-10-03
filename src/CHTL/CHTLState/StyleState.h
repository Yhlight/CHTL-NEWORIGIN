#pragma once

#include "CHTLState.h"
#include <string>
#include <memory>

class ExpressionNode; // Forward declaration

class StyleState : public CHTLState {
public:
    StyleState();
    void handle(CHTLParser& parser, Token token) override;
private:
    std::unique_ptr<ExpressionNode> parseExpression(CHTLParser& parser);
    std::string pendingPropertyName;
    bool expectingValue;
    std::string pendingContextualSelector;
    bool inContextualBlock;
};