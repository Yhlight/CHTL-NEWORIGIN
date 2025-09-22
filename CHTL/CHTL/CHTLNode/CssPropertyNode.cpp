#include "CssPropertyNode.h"
#include "../CHTLGenerator/AstVisitor.h"

namespace CHTL {

void CssPropertyNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
