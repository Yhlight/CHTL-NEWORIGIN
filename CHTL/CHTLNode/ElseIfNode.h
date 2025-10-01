#pragma once

#include "BaseNode.h"
#include "ExpressionNode.h"
#include "StylePropertyNode.h"
#include <vector>
#include <memory>

namespace CHTL {

class ElseIfNode : public BaseNode {
public:
    ElseIfNode(std::unique_ptr<ExpressionNode> condition, std::vector<std::unique_ptr<StylePropertyNode>> body);
    void print(int indent = 0) const override;

    const std::unique_ptr<ExpressionNode>& getCondition() const { return condition; }
    const std::vector<std::unique_ptr<StylePropertyNode>>& getBody() const { return body; }

private:
    std::unique_ptr<ExpressionNode> condition;
    std::vector<std::unique_ptr<StylePropertyNode>> body;
};

} // namespace CHTL