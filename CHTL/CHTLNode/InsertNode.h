#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

enum class InsertPosition {
    AFTER,
    BEFORE,
    REPLACE,
    AT_TOP,
    AT_BOTTOM
};

class InsertNode : public BaseNode {
public:
    InsertPosition position;
    std::string target_selector;

    InsertNode() : BaseNode(NodeType::NODE_INSERT) {}
};

}