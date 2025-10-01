#pragma once

#include "BaseNode.h"
#include "ExpressionNode.h"
#include "StylePropertyNode.h"
#include "ElseIfNode.h"
#include "ElseNode.h"
#include <vector>
#include <memory>

namespace CHTL {

class ElseIfNode;
class ElseNode;

class IfNode : public BaseNode {
public:
    IfNode(std::unique_ptr<ExpressionNode> condition, std::vector<std::unique_ptr<StylePropertyNode>> body);
    void print(int indent = 0) const override;
    void addElseIf(std::unique_ptr<ElseIfNode> elseIfNode);
    void setElse(std::unique_ptr<ElseNode> elseNode);

    const std::unique_ptr<ExpressionNode>& getCondition() const { return condition; }
    const std::vector<std::unique_ptr<StylePropertyNode>>& getBody() const { return body; }
    const std::vector<std::unique_ptr<ElseIfNode>>& getElseIfClauses() const { return elseIfClauses; }
    const std::unique_ptr<ElseNode>& getElseClause() const { return elseClause; }

private:
    std::unique_ptr<ExpressionNode> condition;
    std::vector<std::unique_ptr<StylePropertyNode>> body;
    std::vector<std::unique_ptr<ElseIfNode>> elseIfClauses;
    std::unique_ptr<ElseNode> elseClause;
};

} // namespace CHTL