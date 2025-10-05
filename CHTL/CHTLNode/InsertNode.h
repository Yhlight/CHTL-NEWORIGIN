#pragma once

#include "BaseNode.h"
#include "../Common/ElementTarget.h"
#include <string>
#include <vector>
#include <memory>
#include <optional>

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
    std::optional<ElementTarget> target_selector;

    InsertNode() : BaseNode(NodeType::NODE_INSERT) {}
};

}