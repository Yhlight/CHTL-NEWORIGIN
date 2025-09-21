#ifndef TEXT_NODE_H
#define TEXT_NODE_H

#include "Node.h"

namespace CHTL {

class TextNode : public Node {
public:
    std::string value;

    std::string ToString() const override;
    NodeType GetType() const override { return NodeType::TEXT_NODE; }
};

} // namespace CHTL

#endif // TEXT_NODE_H
