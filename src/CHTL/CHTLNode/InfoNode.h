#pragma once

#include "BaseNode.h"
#include <map>
#include <string>

class InfoNode : public BaseNode {
public:
    std::map<std::string, std::string> metadata;

    InfoNode() : BaseNode() {}

    NodeType getType() const override { return NodeType::Info; }
};