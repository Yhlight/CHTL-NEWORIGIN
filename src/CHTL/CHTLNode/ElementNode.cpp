#include "ElementNode.h"
#include <sstream>

namespace CHTL {

std::string ElementNode::ToString() const {
    std::stringstream ss;
    ss << tagName << " {\n";
    for (const auto& child : children) {
        ss << child->ToString();
    }
    ss << "}\n";
    return ss.str();
}

} // namespace CHTL
