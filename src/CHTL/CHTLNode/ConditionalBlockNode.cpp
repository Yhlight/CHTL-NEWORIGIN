#include "ConditionalBlockNode.h"
#include "StylePropertyNode.h"

ConditionalBlockNode::ConditionalBlockNode(const std::string& type, const std::string& condition)
    : conditionalType(type), condition(condition) {}

NodeType ConditionalBlockNode::getType() const {
    return NodeType::ConditionalBlock;
}

std::string ConditionalBlockNode::getConditionalType() const {
    return conditionalType;
}

std::string ConditionalBlockNode::getCondition() const {
    return condition;
}

void ConditionalBlockNode::setCondition(const std::string& newCondition) {
    condition = newCondition;
}

std::unique_ptr<BaseNode> ConditionalBlockNode::clone() const {
    auto newNode = std::make_unique<ConditionalBlockNode>(conditionalType, condition);
    for (const auto& child : getChildren()) {
        newNode->addChild(child->clone());
    }
    return newNode;
}