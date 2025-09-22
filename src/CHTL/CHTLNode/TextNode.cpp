#include "TextNode.h"
#include <iostream>

namespace CHTL {

    TextNode::TextNode(const std::string& text) : text(text) {}

    const std::string& TextNode::getText() const {
        return text;
    }

    void TextNode::print(int indent) const {
        // Create indentation string
        std::string indentStr;
        for (int i = 0; i < indent; ++i) {
            indentStr += "  ";
        }
        std::cout << indentStr << "TextNode: \"" << text << "\"" << std::endl;
    }

} // namespace CHTL
