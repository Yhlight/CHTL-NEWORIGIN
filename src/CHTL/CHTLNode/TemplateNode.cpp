#include "TemplateNode.h"

namespace CHTL {
TemplateNode::TemplateNode() : BaseNode(NodeType::TEMPLATE) {}
std::string TemplateNode::toString() const { return "Template Node"; }
} // namespace CHTL
