#include "TextNode.h"
#include "NodeVisitor.h"

namespace CHTL {

void TextNode::accept(NodeVisitor& visitor) {
    // Double dispatch for the Visitor pattern.
    visitor.visit(*this);
}

} // namespace CHTL
