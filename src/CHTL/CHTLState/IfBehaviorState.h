#pragma once

#include "CHTLState.h"
#include "../CHTLNode/IfBehaviorNode.h"
#include <string>
#include <vector>
#include <memory>

class IfBehaviorState : public CHTLState {
public:
    IfBehaviorState();
    void handle(CHTLParser& parser, Token token) override;

private:
    enum class Phase {
        // Parsing the initial `if` block
        ParsingIfStart,

        // States for parsing a block with a condition (if, else if)
        AwaitingConditionKeyword,
        AwaitingColon,
        ParsingCondition,
        AwaitingComma,

        // States for parsing a block's properties
        ParsingProperties,

        // Deciding what comes after a block (e.g., an `else`)
        AwaitingNextLink,

        // Parsing an `else` block (which has no condition)
        ParsingElseStart
    };

    Phase phase;

    // Common variables for parsing properties
    bool expectingValue;
    std::string pendingPropertyName;

    // Temporary storage for the block currently being parsed
    std::unique_ptr<ExpressionNode> parsedCondition;
    std::vector<std::unique_ptr<StylePropertyNode>> parsedProperties;

    // Building the node chain
    std::unique_ptr<IfBehaviorNode> headIfNode;
    BaseNode* chainTail = nullptr; // Points to the last If or ElseIf node

    void resetForNewBlock();
};