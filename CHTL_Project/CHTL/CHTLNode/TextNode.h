#pragma once

#include "BaseNode.h"
#include <string>
#include <sstream>

namespace CHTL {

class TextNode : public BaseNode {
public:
    std::string content;

    TextNode(const std::string& text) : content(text) {}

    NodeType getType() const override { return NodeType::Text; }

    std::string toString(int indentation = 0) const override {
        std::stringstream ss;
        std::string indent(indentation, ' ');
        ss << indent << content << std::endl;
        return ss.str();
    }
};

} // namespace CHTL
