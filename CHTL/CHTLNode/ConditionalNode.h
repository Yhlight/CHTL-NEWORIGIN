#ifndef CHTL_CONDITIONAL_NODE_H
#define CHTL_CONDITIONAL_NODE_H

#include "BaseNode.h"
#include "ExpressionNode.h"
#include "StylePropertyNode.h"
#include <vector>
#include <memory>

namespace CHTL {

class ElseNode;
class ElseIfNode;

class IfNode : public BaseNode {
public:
    std::unique_ptr<ExpressionNode> condition;
    std::vector<std::unique_ptr<StylePropertyNode>> body;
    std::unique_ptr<BaseNode> else_branch; // Can be ElseIfNode or ElseNode

    IfNode(std::unique_ptr<ExpressionNode> cond, std::vector<std::unique_ptr<StylePropertyNode>> b)
        : condition(std::move(cond)), body(std::move(b)), else_branch(nullptr) {}

    void print(int indent = 0) const override;
};

class ElseIfNode : public BaseNode {
public:
    std::unique_ptr<ExpressionNode> condition;
    std::vector<std::unique_ptr<StylePropertyNode>> body;
    std::unique_ptr<BaseNode> next_else_branch; // Can be another ElseIfNode or an ElseNode

    ElseIfNode(std::unique_ptr<ExpressionNode> cond, std::vector<std::unique_ptr<StylePropertyNode>> b)
        : condition(std::move(cond)), body(std::move(b)), next_else_branch(nullptr) {}

    void print(int indent = 0) const override;
};

class ElseNode : public BaseNode {
public:
    std::vector<std::unique_ptr<StylePropertyNode>> body;

    explicit ElseNode(std::vector<std::unique_ptr<StylePropertyNode>> b)
        : body(std::move(b)) {}

    void print(int indent = 0) const override;
};

} // namespace CHTL

#endif // CHTL_CONDITIONAL_NODE_H