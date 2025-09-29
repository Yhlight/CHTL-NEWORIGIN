#pragma once

#include "BaseNode.h"
#include "StyleValue.h"
#include <vector>
#include <memory>

// Represents a single case in an if-else if-else chain (e.g., an 'if', 'else if', or 'else' block).
struct ConditionalCase {
    // The condition to evaluate. For 'else' blocks, this will be a StyleValue of type BOOL with a value of true.
    std::unique_ptr<StyleValue> condition;

    // The list of child nodes to include if the condition is met.
    std::vector<std::unique_ptr<BaseNode>> children;

    // Default move operations to ensure vector reallocations are efficient and safe.
    ConditionalCase() = default;
    ConditionalCase(ConditionalCase&&) = default;
    ConditionalCase& operator=(ConditionalCase&&) = default;
    // Keep the copy operations deleted because of unique_ptr.
    ConditionalCase(const ConditionalCase&) = delete;
    ConditionalCase& operator=(const ConditionalCase&) = delete;
};

// Represents a full if-else if-else conditional block in the AST.
class ConditionalNode : public BaseNode {
public:
    ConditionalNode() = default;

    NodeType getType() const override {
        return NodeType::Conditional;
    }

    // The sequence of cases in the conditional chain.
    std::vector<ConditionalCase> cases;
};