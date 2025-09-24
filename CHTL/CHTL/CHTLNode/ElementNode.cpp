#include "ElementNode.h"
#include "../../Util/AstVisitor.h" // Include the full visitor definition
#include <utility>

ElementNode::ElementNode(const std::string& tagName, std::vector<std::unique_ptr<CHTLNode>> children)
    : tagName(tagName), children(std::move(children)) {}

void ElementNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

const std::string& ElementNode::getTagName() const {
    return tagName;
}

const std::vector<std::unique_ptr<CHTLNode>>& ElementNode::getChildren() const {
    return children;
}
