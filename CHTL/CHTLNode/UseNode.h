#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

class UseNode : public BaseNode {
public:
    UseNode(const std::string& useType) : BaseNode(NodeType::NODE_USE), useType(useType) {}

    const std::string& getUseType() const { return useType; }

private:
    std::string useType;
};

}