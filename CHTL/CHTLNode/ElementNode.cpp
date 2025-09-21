#include "ElementNode.h"

namespace CHTL {

void ElementNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
