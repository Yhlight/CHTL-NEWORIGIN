#ifndef CHTL_IMPORT_NODE_H
#define CHTL_IMPORT_NODE_H
#include "BaseNode.h"
namespace CHTL {
class ImportNode : public BaseNode {
public:
    ImportNode();
    std::string toString() const override;
};
} // namespace CHTL
#endif
