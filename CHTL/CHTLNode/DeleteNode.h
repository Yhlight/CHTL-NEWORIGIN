#ifndef CHTL_DELETE_NODE_H
#define CHTL_DELETE_NODE_H

#include "BaseNode.h"
#include "ElementNode.h"
#include <string>
#include <memory>

class DeleteNode : public BaseNode {
public:
    // Constructor for deleting elements, e.g., `delete div;` or `delete div[1];`
    explicit DeleteNode(std::unique_ptr<ElementNode> target)
        : isTemplateDeletion(false), targetElement(std::move(target)) {}

    // Constructor for deleting inherited templates, e.g., `delete @Element Box;`
    explicit DeleteNode(const std::string& templateName)
        : isTemplateDeletion(true), templateNameToDelete(templateName) {}

    std::unique_ptr<BaseNode> clone() const override {
        if (isTemplateDeletion) {
            return std::make_unique<DeleteNode>(templateNameToDelete);
        } else {
            auto clonedTarget = targetElement ? std::unique_ptr<ElementNode>(static_cast<ElementNode*>(targetElement->clone().release())) : nullptr;
            return std::make_unique<DeleteNode>(std::move(clonedTarget));
        }
    }

    std::string toString(int depth = 0) const override { return "DeleteNode"; }
    void accept(NodeVisitor& visitor) override { /* Not visited in final AST */ }
    std::string getNodeType() const override { return "DeleteOperator"; }

    bool getIsTemplateDeletion() const { return isTemplateDeletion; }
    const ElementNode* getTargetElement() const { return targetElement.get(); }
    const std::string& getTemplateNameToDelete() const { return templateNameToDelete; }

private:
    bool isTemplateDeletion;
    std::unique_ptr<ElementNode> targetElement; // Used for element deletion
    std::string templateNameToDelete;           // Used for template deletion
};

#endif // CHTL_DELETE_NODE_H