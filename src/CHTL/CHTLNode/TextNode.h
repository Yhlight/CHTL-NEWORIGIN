#pragma once

#include "BaseNode.h"
#include <string>
#include <sstream>

namespace CHTL {

/**
 * @class TextNode
 * @brief Represents a text node within the AST (e.g., text { "Hello" }).
 */
class TextNode : public BaseNode {
public:
    explicit TextNode(const std::string& content) : content(content) {}

    std::string toString(int indent = 0) const override {
        std::stringstream ss;
        ss << std::string(indent, ' ') << "Text(\"" << content << "\")";
        return ss.str();
    }

    std::string content;
};

} // namespace CHTL
