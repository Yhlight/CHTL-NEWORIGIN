#ifndef CHTL_ELEMENT_NODE_H
#define CHTL_ELEMENT_NODE_H

#include "Node.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

class ElementNode : public Node {
public:
    explicit ElementNode(const std::string& tagName);

    void Accept(AstVisitor& visitor) override;

    std::string tagName;
    std::map<std::string, std::string> attributes;
    std::vector<std::unique_ptr<Node>> children;
};

} // namespace CHTL

#endif // CHTL_ELEMENT_NODE_H
