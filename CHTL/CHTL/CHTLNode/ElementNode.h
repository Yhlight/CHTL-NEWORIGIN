#ifndef CHTL_ELEMENT_NODE_H
#define CHTL_ELEMENT_NODE_H

#include "CHTLNode.h"
#include <string>
#include <vector>
#include <memory>

// Forward declaration
class AstVisitor;

class ElementNode : public CHTLNode {
public:
    ElementNode(const std::string& tagName, std::vector<std::unique_ptr<CHTLNode>> children);

    void accept(AstVisitor& visitor) override;

    const std::string& getTagName() const;
    const std::vector<std::unique_ptr<CHTLNode>>& getChildren() const;

private:
    std::string tagName;
    std::vector<std::unique_ptr<CHTLNode>> children;
};

#endif // CHTL_ELEMENT_NODE_H
