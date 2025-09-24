#include "ElementNode.h"
#include "NodeVisitor.h"

namespace CHTL {

void ElementNode::accept(NodeVisitor& visitor) {
    // This is the core of the Visitor pattern's double dispatch.
    // We call the 'visit' method on the visitor that is specific to ElementNode.
    visitor.visit(*this);
}

} // namespace CHTL
