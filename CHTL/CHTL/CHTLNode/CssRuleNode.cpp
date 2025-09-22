#include "CssRuleNode.h"
#include "../CHTLGenerator/AstVisitor.h"

namespace CHTL {

void CssRuleNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
