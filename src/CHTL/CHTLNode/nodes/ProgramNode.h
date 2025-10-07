#ifndef CHTL_PROGRAM_NODE_H
#define CHTL_PROGRAM_NODE_H

#include "../BaseNode.h"

namespace CHTL {

class ProgramNode : public BaseNode {
public:
    ProgramNode();

    void accept(NodeVisitor& visitor) override;
    String toString(int indent = 0) const override;
    SharedPtr<BaseNode> clone() const override;
};

} // namespace CHTL

#endif // CHTL_PROGRAM_NODE_H
