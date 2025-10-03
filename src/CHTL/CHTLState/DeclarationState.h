#pragma once

#include "CHTLState.h"
#include <string>

class DeclarationState : public CHTLState {
public:
    DeclarationState();
    void handle(CHTLParser& parser, Token token) override;

private:
    enum class Phase {
        AwaitingKeyword,      // Expecting [Custom] or [Template]
        AwaitingAt,           // Expecting @
        AwaitingType,         // Expecting Style, Element, etc.
        AwaitingName,         // Expecting the name of the template/custom
        AwaitingRightBracket, // Expecting ]
        AwaitingLeftBrace,    // Expecting {
    };

    Phase phase;
    std::string declarationType; // "Template" or "Custom"
    std::string blockType;       // "Style", "Element", "Var"
    std::string blockName;
};