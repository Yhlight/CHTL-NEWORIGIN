#pragma once

#include "BaseNode.h"
#include <string>
#include <sstream>

namespace CHTL {

class CommentNode : public BaseNode {
public:
    std::string content;

    CommentNode(const std::string& text) : content(text) {}

    NodeType getType() const override { return NodeType::Comment; }

    std::string toString(int indentation = 0) const override {
        std::stringstream ss;
        std::string indent(indentation, ' ');
        // This toString is for debugging. The actual generation happens in the Generator.
        ss << indent << "<!--" << content << "-->" << std::endl;
        return ss.str();
    }
};

} // namespace CHTL
