#ifndef CHTL_STYLE_NODE_H
#define CHTL_STYLE_NODE_H

#include "BaseNode.h"
#include "StylePropertyNode.h"
#include "SelectorBlockNode.h"
#include "TemplateUsageNode.h"
#include <vector>
#include <memory>

namespace CHTL {

class StyleNode : public BaseNode {
public:
    void addProperty(std::unique_ptr<StylePropertyNode> property);
    void addSelectorBlock(std::unique_ptr<SelectorBlockNode> selectorBlock);
    void addTemplateUsage(std::unique_ptr<TemplateUsageNode> templateUsage);

    const std::vector<std::unique_ptr<StylePropertyNode>>& getProperties() const;
    const std::vector<std::unique_ptr<SelectorBlockNode>>& getSelectorBlocks() const;
    const std::vector<std::unique_ptr<TemplateUsageNode>>& getTemplateUsages() const;

    void print(int indent = 0) const override;

private:
    std::vector<std::unique_ptr<StylePropertyNode>> properties;
    std::vector<std::unique_ptr<SelectorBlockNode>> selector_blocks;
    std::vector<std::unique_ptr<TemplateUsageNode>> template_usages;
};

} // namespace CHTL

#endif // CHTL_STYLE_NODE_H