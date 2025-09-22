#pragma once

#include <memory>
#include <vector>

namespace CHTL {

    class BaseNode {
    public:
        virtual ~BaseNode() = default;
        // A virtual print method for debugging the AST
        virtual void print(int indent = 0) const = 0;
    };

} // namespace CHTL
