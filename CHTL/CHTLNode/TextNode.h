#ifndef CHTL_TEXT_NODE_H
#define CHTL_TEXT_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class TextNode : public BaseNode {
public:
    explicit TextNode(const std::string& value) : value(value) {}

    const std::string& getValue() const { return value; }

private:
    std::string value;
};

} // namespace CHTL

#endif // CHTL_TEXT_NODE_H