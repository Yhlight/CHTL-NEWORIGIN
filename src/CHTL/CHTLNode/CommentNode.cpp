#include "CommentNode.h"
#include <iostream>

namespace CHTL {
    CommentNode::CommentNode(const std::string& text) : text(text) {}

    void CommentNode::print(int indent) const {
        std::string indentStr;
        for (int i = 0; i < indent; ++i) { indentStr += "  "; }
        std::cout << indentStr << "CommentNode: #" << text << std::endl;
    }
}
