#ifndef TEXT_NODE_H
#define TEXT_NODE_H

#include "CHTL/CHTLNode/BaseNode.h"
#include <string>

namespace CHTL {

class TextNode : public BaseNode {
public:
    std::string content;
};

} // namespace CHTL

#endif // TEXT_NODE_H
