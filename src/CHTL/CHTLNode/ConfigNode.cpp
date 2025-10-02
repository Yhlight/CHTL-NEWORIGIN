#include "ConfigNode.h"
namespace CHTL {
ConfigNode::ConfigNode() : BaseNode(NodeType::CONFIG) {}
std::string ConfigNode::toString() const { return "Config Node"; }
}
