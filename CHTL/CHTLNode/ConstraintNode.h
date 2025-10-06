#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>

namespace CHTL {

    enum class ConstraintType {
        // Precise constraints
        HTML_TAG,          // e.g., except span;
        NAMED_TEMPLATE,    // e.g., except [Template] @Style MyColors;
        NAMED_CUSTOM,      // e.g., except [Custom] @Element Box;

        // Type constraints
        ANY_HTML_TAG,      // e.g., except @Html;
        ANY_TEMPLATE,      // e.g., except [Template];
        ANY_CUSTOM,        // e.g., except [Custom];
        TEMPLATE_SUBTYPE,  // e.g., except [Template] @Style;
        CUSTOM_SUBTYPE     // e.g., except [Custom] @Element;
    };

    struct Constraint {
        ConstraintType type;
        std::string subType; // For subtype constraints like @Style, @Element, @Var
        std::string value;   // For precise constraints (tag name or entity name)
    };

    class ConstraintNode : public BaseNode {
    public:
        std::vector<Constraint> constraints;

        ConstraintNode() : BaseNode(NodeType::NODE_CONSTRAINT) {}

        void addConstraint(const Constraint& constraint) {
            constraints.push_back(constraint);
        }
    };

}