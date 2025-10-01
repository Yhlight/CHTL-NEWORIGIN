#ifndef CHTL_CONDITIONAL_NODE_H
#define CHTL_CONDITIONAL_NODE_H

#include "BaseNode.h"
#include "ExpressionNode.h"
#include "StylePropertyNode.h"
#include <vector>
#include <memory>

namespace CHTL {

class ConditionalNode : public BaseNode {
public:
    ConditionalNode(std::unique_ptr<ExpressionNode> condition, std::vector<std::unique_ptr<StylePropertyNode>> body);
    void print(int indent = 0) const override;

    std::unique_ptr<ExpressionNode> condition;
    std::vector<std::unique_ptr<StylePropertyNode>> body;
    std::unique_ptr<ConditionalNode> else_branch;
};

} // namespace CHTL

#endif // CHTL_CONDITIONAL_NODE_H