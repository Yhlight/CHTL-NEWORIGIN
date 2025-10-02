#ifndef CHTL_CUSTOM_NODE_H
#define CHTL_CUSTOM_NODE_H
#include "BaseNode.h"
namespace CHTL {
class CustomNode : public BaseNode {
public:
    CustomNode();
    std::string toString() const override;
};
} // namespace CHTL
#endif
