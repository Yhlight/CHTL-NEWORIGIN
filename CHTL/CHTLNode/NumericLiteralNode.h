#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

class NumericLiteralNode : public BaseNode {
public:
    std::string value;
    std::string unit;

    NumericLiteralNode(const std::string& value, const std::string& unit = "")
        : BaseNode(NodeType::NODE_NUMERIC_LITERAL), value(value), unit(unit) {}

    std::string ToString(const std::string& indent = "", bool isLast = true) const override {
        return indent + (isLast ? "`-- " : "|-- ") + "NumericLiteralNode: " + value + unit + "\n";
    }
};

}