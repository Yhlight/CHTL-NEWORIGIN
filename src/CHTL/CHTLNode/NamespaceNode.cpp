#include "NamespaceNode.h"
namespace CHTL {
NamespaceNode::NamespaceNode() : BaseNode(NodeType::NAMESPACE) {}
std::string NamespaceNode::toString() const { return "Namespace Node"; }
}
