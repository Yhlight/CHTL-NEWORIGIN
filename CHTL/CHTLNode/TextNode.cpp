#include "TextNode.h"
#include <iostream>

namespace CHTL {

void TextNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "\"" << text << "\"" << std::endl;
}

const std::string& TextNode::getValue() const {
    return text;
}

} // namespace CHTL