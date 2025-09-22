#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

class OriginNode : public BaseNode {
public:
    std::string typeName; // e.g., "Html", "Style"
    std::string content;

    OriginNode(const std::string& type, const std::string& c) : typeName(type), content(c) {}

    NodeType getType() const override { return NodeType::Origin; }

    std::string toString(int indentation = 0) const override {
        // The generator will just print the content.
        return content;
    }
};

} // namespace CHTL
