#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

class BooleanLiteralNode : public BaseNode {
public:
    bool value;

    BooleanLiteralNode(bool val)
        : BaseNode(NodeType::NODE_BOOLEAN_LITERAL), value(val) {}

    std::string ToString(const std::string& indent = "", bool isLast = true) const override {
        return indent + (isLast ? "`-- " : "|-- ") + "BooleanLiteralNode: " + (value ? "true" : "false") + "\n";
    }
};

}