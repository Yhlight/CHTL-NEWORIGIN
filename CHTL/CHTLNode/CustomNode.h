#ifndef CHTL_CUSTOMNODE_H
#define CHTL_CUSTOMNODE_H

#include "BaseNode.h"
#include "TemplateNode.h" // For reusing TemplateType
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class CustomNode : public BaseNode {
public:
    CustomNode(TemplateType customType, const std::string& name)
        : BaseNode(NodeType::CUSTOM), customType(customType), name(name) {}

    TemplateType getCustomType() const { return customType; }
    const std::string& getName() const { return name; }
    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const { return children; }

    void addChild(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }

    void print(int indent = 0) const override;
    std::unique_ptr<BaseNode> clone() const override;

private:
    TemplateType customType;
    std::string name;
    std::vector<std::unique_ptr<BaseNode>> children;
};

} // namespace CHTL

#endif // CHTL_CUSTOMNODE_H