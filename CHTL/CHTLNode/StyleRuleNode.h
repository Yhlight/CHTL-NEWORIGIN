#ifndef CHTL_STYLERULENODE_H
#define CHTL_STYLERULENODE_H

#include "BaseNode.h"
#include "StylePropertyNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class StyleRuleNode : public BaseNode {
public:
    StyleRuleNode(const std::string& selector);
    ~StyleRuleNode() override = default;
    StyleRuleNode(StyleRuleNode&&) noexcept = default;
    StyleRuleNode& operator=(StyleRuleNode&&) noexcept = default;

    void print(int indent = 0) const override;
    std::string getSelector() const;
    const std::vector<std::unique_ptr<StylePropertyNode>>& getProperties() const;
    void addProperty(std::unique_ptr<StylePropertyNode> property);

private:
    std::string selector;
    std::vector<std::unique_ptr<StylePropertyNode>> properties;
};

} // namespace CHTL

#endif // CHTL_STYLERULENODE_H