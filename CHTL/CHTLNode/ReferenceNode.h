#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

class ReferenceNode : public BaseNode {
public:
    std::string selector;
    std::string propertyName;

    ReferenceNode(const std::string& sel, const std::string& prop)
        : BaseNode(NodeType::NODE_REFERENCE), selector(sel), propertyName(prop) {}

    std::string ToString(const std::string& indent = "", bool isLast = true) const override {
        return indent + (isLast ? "`-- " : "|-- ") + "ReferenceNode: " + selector + "." + propertyName + "\n";
    }
};

}