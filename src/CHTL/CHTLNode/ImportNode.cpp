#include "ImportNode.h"
namespace CHTL {
ImportNode::ImportNode() : BaseNode(NodeType::IMPORT) {}
std::string ImportNode::toString() const { return "Import Node"; }
}
