#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>

namespace CHTL {

class DeleteNode : public BaseNode {
public:
    std::vector<std::string> targets;

    DeleteNode() {
        type = NodeType::NODE_DELETE;
    }

    void addTarget(const std::string& target) {
        targets.push_back(target);
    }
};

}