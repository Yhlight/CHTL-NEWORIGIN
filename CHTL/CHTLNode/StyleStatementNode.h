#pragma once

#include "Node.h"

// This will be a base class for anything that can appear inside a style block,
// e.g., a property declaration or a template usage.
class StyleStatementNode : public Node {
public:
    void accept(Visitor& visitor) override {
        // This might be visited by a style-specific visitor in the future
    }
};

using StyleStatementList = std::vector<std::unique_ptr<StyleStatementNode>>;
