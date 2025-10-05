#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

enum class CustomType {
    STYLE,
    ELEMENT,
    VAR
};

class CustomNode : public BaseNode {
public:
    CustomNode(const std::string& name, CustomType customType) : BaseNode(NodeType::NODE_CUSTOM), name(name), customType(customType) {}

    const std::string& getName() const { return name; }
    CustomType getCustomType() const { return customType; }

private:
    std::string name;
    CustomType customType;
};

}