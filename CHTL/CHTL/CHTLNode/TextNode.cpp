#include "TextNode.h"
#include "../CHTLGenerator/AstVisitor.h"

namespace CHTL {

void TextNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
