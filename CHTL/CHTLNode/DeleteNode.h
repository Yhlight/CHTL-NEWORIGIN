#pragma once

#include "BaseNode.h"
#include "../Common/ElementTarget.h"
#include <vector>

namespace CHTL {

class DeleteNode : public BaseNode {
public:
    std::vector<ElementTarget> targets;

    DeleteNode() : BaseNode(NodeType::NODE_DELETE) {}

    void addTarget(const ElementTarget& target) {
        targets.push_back(target);
    }
};

}