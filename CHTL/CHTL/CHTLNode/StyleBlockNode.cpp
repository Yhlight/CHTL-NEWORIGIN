#include "StyleBlockNode.h"
#include "../CHTLGenerator/AstVisitor.h"

namespace CHTL {

void StyleBlockNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
