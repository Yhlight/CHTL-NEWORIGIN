#include "AttributeNode.h"
#include "../CHTLGenerator/AstVisitor.h"

namespace CHTL {

void AttributeNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
