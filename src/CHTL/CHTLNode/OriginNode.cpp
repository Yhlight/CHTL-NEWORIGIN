#include "OriginNode.h"
namespace CHTL {
OriginNode::OriginNode() : BaseNode(NodeType::ORIGIN) {}
std::string OriginNode::toString() const { return "Origin Node"; }
}
