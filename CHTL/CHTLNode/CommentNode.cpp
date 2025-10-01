#include "CommentNode.h"
#include <iostream>

namespace CHTL {

CommentNode::CommentNode(const std::string& value) : value(value) {}

void CommentNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "# " << value << std::endl;
}

const std::string& CommentNode::getValue() const {
    return value;
}

} // namespace CHTL