#ifndef CHTL_TEMPLATENODE_H
#define CHTL_TEMPLATENODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

enum class TemplateType {
    STYLE,
    ELEMENT,
    VAR
};

class TemplateNode : public BaseNode {
public:
    TemplateNode(TemplateType templateType, const std::string& name)
        : BaseNode(NodeType::TEMPLATE), templateType(templateType), name(name) {}

    TemplateType getTemplateType() const { return templateType; }
    const std::string& getName() const { return name; }
    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const { return children; }

    void addChild(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }
    void print(int indent = 0) const override;
    std::unique_ptr<BaseNode> clone() const override;

private:
    TemplateType templateType;
    std::string name;
    std::vector<std::unique_ptr<BaseNode>> children;
};

} // namespace CHTL

#endif // CHTL_TEMPLATENODE_H