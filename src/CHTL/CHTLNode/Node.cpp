#include "Node.h"
#include <sstream>

namespace CHTL {

std::string Program::ToString() const {
    std::stringstream ss;
    for (const auto& child : children) {
        ss << child->ToString();
    }
    return ss.str();
}

} // namespace CHTL
