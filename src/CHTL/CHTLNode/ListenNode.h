#pragma once

#include "BaseNode.h"
#include "EnhancedSelectorNode.h"
#include <string>
#include <map>
#include <memory>

class ListenNode : public BaseNode {
public:
    std::unique_ptr<EnhancedSelectorNode> target;
    std::map<std::string, std::string> events;

    ListenNode(std::unique_ptr<EnhancedSelectorNode> target)
        : target(std::move(target)) {}

    NodeType getType() const override { return NodeType::Listen; }
};