#include "CustomNode.h"
namespace CHTL {
CustomNode::CustomNode() : BaseNode(NodeType::CUSTOM) {}
std::string CustomNode::toString() const { return "Custom Node"; }
}
