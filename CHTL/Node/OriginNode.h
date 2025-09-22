#pragma once

#include "BaseNode.h"
#include <string>
#include <sstream>

namespace CHTL {

class OriginNode : public BaseNode {
public:
    std::string type_name; // e.g., "Html", "Style", "JavaScript"
    std::string content;

    OriginNode(const std::string& type, const std::string& c) : type_name(type), content(c) {}

    NodeType getType() const override {
        return NodeType::Origin;
    }

    std::string toString(int indent = 0) const override {
        std::string ind(indent * 2, ' ');
        std::stringstream ss;
        ss << ind << "OriginNode(@" << type_name << ") {\n";
        ss << content << "\n"; // Note: content is not indented
        ss << ind << "}\n";
        return ss.str();
    }
};

} // namespace CHTL
