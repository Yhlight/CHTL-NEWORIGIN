#pragma once

#include "BaseNode.h"
#include "ExpressionNode.h"
#include "StylePropertyNode.h"
#include <memory>
#include <vector>

// Forward declaration for the chain
class ElseIfBehaviorNode;
class ElseBehaviorNode;

class IfBehaviorNode : public BaseNode {
public:
    IfBehaviorNode(std::unique_ptr<ExpressionNode> condition,
                   std::vector<std::unique_ptr<StylePropertyNode>> properties)
        : condition(std::move(condition)), properties(std::move(properties)),
          else_if_chain(nullptr), else_node(nullptr) {}

    NodeType getType() const override { return NodeType::IfBehavior; }

    const ExpressionNode *getCondition() const { return condition.get(); }
    const std::vector<std::unique_ptr<StylePropertyNode>> &
    getProperties() const {
        return properties;
    }

    std::vector<std::unique_ptr<StylePropertyNode>> takeProperties() {
        return std::move(properties);
    }

    void setElseIfChain(std::unique_ptr<ElseIfBehaviorNode> next_node) {
        else_if_chain = std::move(next_node);
    }

    void setElseNode(std::unique_ptr<ElseBehaviorNode> next_node) {
        else_node = std::move(next_node);
    }

    const ElseIfBehaviorNode *getElseIfChain() const {
        return else_if_chain.get();
    }
    ElseIfBehaviorNode *getElseIfChain() {
        return else_if_chain.get();
    }
    const ElseBehaviorNode *getElseNode() const { return else_node.get(); }

private:
    std::unique_ptr<ExpressionNode> condition;
    std::vector<std::unique_ptr<StylePropertyNode>> properties;
    std::unique_ptr<ElseIfBehaviorNode> else_if_chain;
    std::unique_ptr<ElseBehaviorNode> else_node;
};

class ElseIfBehaviorNode : public BaseNode {
public:
    ElseIfBehaviorNode(
        std::unique_ptr<ExpressionNode> condition,
        std::vector<std::unique_ptr<StylePropertyNode>> properties)
        : condition(std::move(condition)), properties(std::move(properties)),
          next_else_if(nullptr), else_node(nullptr) {}

    NodeType getType() const override { return NodeType::ElseIfBehavior; }

    const ExpressionNode *getCondition() const { return condition.get(); }
    const std::vector<std::unique_ptr<StylePropertyNode>> &
    getProperties() const {
        return properties;
    }

    std::vector<std::unique_ptr<StylePropertyNode>> takeProperties() {
        return std::move(properties);
    }

    void setNextElseIf(std::unique_ptr<ElseIfBehaviorNode> next_node) {
        next_else_if = std::move(next_node);
    }

    void setElseNode(std::unique_ptr<ElseBehaviorNode> next_node) {
        else_node = std::move(next_node);
    }

    const ElseIfBehaviorNode *getNextElseIf() const {
        return next_else_if.get();
    }
    ElseIfBehaviorNode *getNextElseIf() {
        return next_else_if.get();
    }
    const ElseBehaviorNode *getElseNode() const { return else_node.get(); }

private:
    std::unique_ptr<ExpressionNode> condition;
    std::vector<std::unique_ptr<StylePropertyNode>> properties;
    std::unique_ptr<ElseIfBehaviorNode> next_else_if;
    std::unique_ptr<ElseBehaviorNode> else_node;
};

class ElseBehaviorNode : public BaseNode {
public:
    ElseBehaviorNode(
        std::vector<std::unique_ptr<StylePropertyNode>> properties)
        : properties(std::move(properties)) {}

    NodeType getType() const override { return NodeType::ElseBehavior; }

    const std::vector<std::unique_ptr<StylePropertyNode>> &
    getProperties() const {
        return properties;
    }

    std::vector<std::unique_ptr<StylePropertyNode>> takeProperties() {
        return std::move(properties);
    }

private:
    std::vector<std::unique_ptr<StylePropertyNode>> properties;
};