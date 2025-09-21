#ifndef ELEMENT_NODE_H
#define ELEMENT_NODE_H

#include "Node.h"

namespace CHTL {

class ElementNode : public Node {
public:
    std::string tagName;
    std::vector<std::unique_ptr<Node>> children;
    // Attributes will be added later

    std::string ToString() const override;
    NodeType GetType() const override { return NodeType::ELEMENT_NODE; }
};

} // namespace CHTL

#endif // ELEMENT_NODE_H
