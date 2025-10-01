#ifndef CHTL_IF_NODE_H
#define CHTL_IF_NODE_H

#include "BaseNode.h"
#include "ExpressionNode.h"
#include "StylePropertyNode.h"
#include <vector>
#include <memory>

namespace CHTL {

class IfNode : public BaseNode {
public:
    // If condition is nullptr, it represents an 'else' block.
    IfNode(std::unique_ptr<ExpressionNode> condition, std::vector<std::unique_ptr<StylePropertyNode>> body);

    void setElseBranch(std::unique_ptr<IfNode> else_branch);

    void print(int indent = 0) const override;

    const ExpressionNode* getCondition() const;
    const std::vector<std::unique_ptr<StylePropertyNode>>& getBody() const;
    const IfNode* getElseBranch() const;

private:
    std::unique_ptr<ExpressionNode> m_condition;
    std::vector<std::unique_ptr<StylePropertyNode>> m_body;
    std::unique_ptr<IfNode> m_else_branch;
};

} // namespace CHTL

#endif // CHTL_IF_NODE_H