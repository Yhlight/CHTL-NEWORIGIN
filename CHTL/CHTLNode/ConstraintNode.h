#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>

namespace CHTL {

    enum class ConstraintType {
        TAG_NAME,
        TEMPLATE,
        CUSTOM,
        TYPE_HTML,
        TYPE_TEMPLATE,
        TYPE_CUSTOM
    };

    struct Constraint {
        ConstraintType type;
        std::string value;
    };

    class ConstraintNode : public BaseNode {
    public:
        std::vector<Constraint> constraints;

        ConstraintNode() { type = CHTL::NodeType::NODE_CONSTRAINT; }

        void addConstraint(const Constraint& constraint) {
            constraints.push_back(constraint);
        }
    };

}