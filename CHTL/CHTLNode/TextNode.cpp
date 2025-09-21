#include "TextNode.h"

namespace CHTL {

void TextNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
