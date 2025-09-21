#include "ProgramNode.h"

namespace CHTL {

void ProgramNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
