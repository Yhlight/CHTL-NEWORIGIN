#pragma once

#include "BaseNode.h"
#include <map>
#include <string>
#include <vector>

class InfoNode : public BaseNode {
public:
    std::map<std::string, std::string> metadata;
    std::map<std::string, std::vector<std::string>> exports;
    bool exportBlockExists = false;

    InfoNode() : BaseNode() {}

    NodeType getType() const override { return NodeType::Info; }
};