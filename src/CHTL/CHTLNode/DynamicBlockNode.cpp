#include "DynamicBlockNode.h"
#include "StylePropertyNode.h"

DynamicBlockNode::DynamicBlockNode(const std::string& type, const std::string& condition)
    : conditionalType(type), condition(condition) {}

NodeType DynamicBlockNode::getType() const {
    return NodeType::DynamicBlock;
}

std::string DynamicBlockNode::getConditionalType() const {
    return conditionalType;
}

std::string DynamicBlockNode::getCondition() const {
    return condition;
}

std::unique_ptr<BaseNode> DynamicBlockNode::clone() const {
    auto newNode = std::make_unique<DynamicBlockNode>(conditionalType, condition);
    for (const auto& child : getChildren()) {
        newNode->addChild(child->clone());
    }
    return newNode;
}