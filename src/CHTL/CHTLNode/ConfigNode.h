#ifndef CHTL_CONFIG_NODE_H
#define CHTL_CONFIG_NODE_H
#include "BaseNode.h"
namespace CHTL {
class ConfigNode : public BaseNode {
public:
    ConfigNode();
    std::string toString() const override;
};
} // namespace CHTL
#endif
