#pragma once

#include <memory>
#include <vector>
#include "NodeVisitor.h"

namespace CHTL {

class BaseNode {
public:
    virtual ~BaseNode() = default;

    // The accept method for the Visitor pattern.
    // Concrete node types will implement this to call the correct
    // visit method on the visitor.
    virtual void accept(NodeVisitor& visitor) = 0;

    void addChild(std::shared_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }

    const std::vector<std::shared_ptr<BaseNode>>& getChildren() const {
        return children;
    }

protected:
    std::vector<std::shared_ptr<BaseNode>> children;
};

} // namespace CHTL
