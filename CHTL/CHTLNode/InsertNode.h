#ifndef CHTL_INSERT_NODE_H
#define CHTL_INSERT_NODE_H

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
    InsertNode(InsertPosition position, const std::string& selector, std::vector<std::unique_ptr<BaseNode>> children);

    void print(int indent = 0) const override;
    InsertPosition getPosition() const;
    const std::string& getSelector() const;
    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const;

private:
    InsertPosition m_position;
    std::string m_selector;
    std::vector<std::unique_ptr<BaseNode>> m_children;
};

} // namespace CHTL

#endif // CHTL_INSERT_NODE_H