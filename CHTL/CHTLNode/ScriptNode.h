#ifndef CHTL_SCRIPT_NODE_H
#define CHTL_SCRIPT_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class ScriptNode : public BaseNode {
public:
    ScriptNode() = default;

    void addChild(std::unique_ptr<BaseNode> child);
    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const;
    void print(int indent = 0) const override;

private:
    std::vector<std::unique_ptr<BaseNode>> m_children;
};

} // namespace CHTL

#endif // CHTL_SCRIPT_NODE_H