#ifndef CHTL_NAMESPACE_NODE_H
#define CHTL_NAMESPACE_NODE_H
#include "BaseNode.h"
namespace CHTL {
class NamespaceNode : public BaseNode {
public:
    NamespaceNode();
    std::string toString() const override;
};
} // namespace CHTL
#endif
