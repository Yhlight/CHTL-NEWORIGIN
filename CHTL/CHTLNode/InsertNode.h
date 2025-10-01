#ifndef CHTL_INSERTNODE_H
#define CHTL_INSERTNODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

enum class InsertPosition {
    AFTER,
    BEFORE,
    REPLACE,
    AT_TOP,
    AT_BOTTOM
};

class InsertNode : public BaseNode {
public:
    InsertNode(InsertPosition position, const std::string& target)
        : BaseNode(NodeType::INSERT_ACTION), position(position), target(target) {}

    InsertPosition getPosition() const { return position; }
    const std::string& getTarget() const { return target; }
    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const { return children; }

    void addChild(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }

    void print(int indent = 0) const override;
    std::unique_ptr<BaseNode> clone() const override;

private:
    InsertPosition position;
    std::string target;
    std::vector<std::unique_ptr<BaseNode>> children;
};

} // namespace CHTL

#endif // CHTL_INSERTNODE_H