#pragma once

#include "BaseNode.h"
#include <string>
#include <sstream>

namespace CHTL {

class TextNode : public BaseNode {
public:
    std::string content;

    TextNode(const std::string& text) : content(text) {}

    NodeType getType() const override {
        return NodeType::Text;
    }

    std::string toString(int indent = 0) const override {
        std::string ind(indent * 2, ' ');
        std::stringstream ss;
        // Simple string escaping for display
        std::string display_content = content;
        size_t pos = 0;
        while ((pos = display_content.find("\"", pos)) != std::string::npos) {
            display_content.replace(pos, 1, "\\\"");
            pos += 2;
        }
        ss << ind << "TextNode(\"" << display_content << "\")\n";
        return ss.str();
    }
};

} // namespace CHTL
