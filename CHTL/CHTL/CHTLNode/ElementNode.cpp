#include "ElementNode.h"
#include "../CHTLGenerator/AstVisitor.h"

namespace CHTL {

void ElementNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
