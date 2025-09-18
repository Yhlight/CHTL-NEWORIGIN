#pragma once

#include "../BaseNode.h"

namespace CHTL {
    // This is the abstract base class for all nodes
    // that represent an expression, e.g., in a style property value.
    class ExprNode : public BaseNode {
        // The accept method will be inherited and implemented by concrete expression nodes.
    };
}
