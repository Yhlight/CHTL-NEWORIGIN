#ifndef CHTL_DOCUMENT_NODE_H
#define CHTL_DOCUMENT_NODE_H

#include "BaseNode.h"
#include <vector>
#include <memory>

namespace CHTL {

class DocumentNode : public BaseNode {
public:
    void print(int indent = 0) const override;

    std::vector<std::unique_ptr<BaseNode>> children;
    bool useHtml5 = false;
};

} // namespace CHTL

#endif // CHTL_DOCUMENT_NODE_H