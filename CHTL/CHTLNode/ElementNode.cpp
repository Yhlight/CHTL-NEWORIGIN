#include "ElementNode.h"
#include "AstVisitor.h"

namespace CHTL {

ElementNode::ElementNode(const std::string& tagName) : tagName(tagName) {}

void ElementNode::Accept(AstVisitor& visitor) {
    visitor.Visit(*this);
}

} // namespace CHTL
