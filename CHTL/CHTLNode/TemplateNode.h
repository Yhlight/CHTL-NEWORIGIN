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

    void print(int indent = 0) const override;
    std::string getTemplateType() const;
    std::string getTemplateName() const;
    const std::vector<std::unique_ptr<StylePropertyNode>>& getProperties() const;
    void addProperty(std::unique_ptr<StylePropertyNode> property);

private:
    std::string templateType;
    std::string templateName;
    std::vector<std::unique_ptr<StylePropertyNode>> properties;
};

} // namespace CHTL

#endif // CHTL_TEMPLATENODE_H