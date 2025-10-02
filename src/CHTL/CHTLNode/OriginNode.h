#ifndef CHTL_ORIGIN_NODE_H
#define CHTL_ORIGIN_NODE_H
#include "BaseNode.h"
namespace CHTL {
class OriginNode : public BaseNode {
public:
    OriginNode();
    std::string toString() const override;
};
} // namespace CHTL
#endif
