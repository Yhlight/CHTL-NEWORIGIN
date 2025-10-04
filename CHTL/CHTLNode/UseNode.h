#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

class UseNode : public BaseNode {
public:
    UseNode(const std::string& useType) : useType(useType) {
        this->type = NodeType::NODE_USE;
    }

    const std::string& getUseType() const { return useType; }

private:
    std::string useType;
};

}