#ifndef CHTL_TEMPLATENODE_H
#define CHTL_TEMPLATENODE_H

#include "BaseNode.h"
#include "StylePropertyNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class TemplateNode : public BaseNode {
public:
    TemplateNode(const std::string& type, const std::string& name);
    ~TemplateNode() override = default;
    TemplateNode(TemplateNode&&) noexcept = default;
    TemplateNode& operator=(TemplateNode&&) noexcept = default;

    void print(int indent = 0) const override;
    std::string getTemplateType() const;
    std::string getTemplateName() const;

    // For Style Templates
    const std::vector<std::unique_ptr<StylePropertyNode>>& getProperties() const;
    void addProperty(std::unique_ptr<StylePropertyNode> property);

    // For Element Templates
    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const;
    void addChild(std::unique_ptr<BaseNode> child);

private:
    std::string templateType;
    std::string templateName;
    // A template can hold either properties (for @Style) or children (for @Element)
    std::vector<std::unique_ptr<StylePropertyNode>> properties;
    std::vector<std::unique_ptr<BaseNode>> children;
};

} // namespace CHTL

#endif // CHTL_TEMPLATENODE_H