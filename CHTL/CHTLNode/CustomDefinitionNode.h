#ifndef CHTL_CUSTOM_DEFINITION_NODE_H
#define CHTL_CUSTOM_DEFINITION_NODE_H

#include "BaseNode.h"
#include "TemplateDefinitionNode.h" // For TemplateType
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class CustomDefinitionNode : public BaseNode {
public:
    CustomDefinitionNode(TemplateType type, const std::string& name);

    void addChild(std::unique_ptr<BaseNode> child);
    void print(int indent = 0) const override;

    TemplateType getTemplateType() const;
    const std::string& getName() const;
    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const;

private:
    TemplateType m_type;
    std::string m_name;
    std::vector<std::unique_ptr<BaseNode>> m_children;
};

} // namespace CHTL

#endif // CHTL_CUSTOM_DEFINITION_NODE_H