#ifndef ELEMENT_NODE_H
#define ELEMENT_NODE_H

#include "CHTL/CHTLNode/BaseNode.h"
#include <string>
#include <vector>
#include <map>

namespace CHTL {

struct Attribute {
    std::string name;
    std::string value;
};

class ElementNode : public BaseNode {
public:
    std::string tagName;
    std::vector<Attribute> attributes;
    std::vector<std::unique_ptr<BaseNode>> children;
};

} // namespace CHTL

#endif // ELEMENT_NODE_H
