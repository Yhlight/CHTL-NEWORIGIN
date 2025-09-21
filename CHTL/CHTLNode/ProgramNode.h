#pragma once

#include "BaseNode.h"
#include "../CHTLGenerator/Visitor.h"
#include <vector>
#include <memory>

namespace CHTL {

// Represents the root of the entire CHTL document.
// It holds a list of all top-level statements/nodes.
class ProgramNode : public BaseNode {
public:
    std::vector<std::unique_ptr<BaseNode>> children;

    void accept(Visitor& visitor) override;
};

} // namespace CHTL
