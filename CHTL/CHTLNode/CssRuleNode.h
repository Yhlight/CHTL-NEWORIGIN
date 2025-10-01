#ifndef CHTL_CSSRULENODE_H
#define CHTL_CSSRULENODE_H

#include <string>
#include <vector>
#include <memory>
#include "BaseNode.h"
#include "StylePropertyNode.h"

namespace CHTL {

class CssRuleNode : public BaseNode {
public:
    CssRuleNode(const std::string& selector);

    void addProperty(std::unique_ptr<StylePropertyNode> property);
    const std::string& getSelector() const;
    const std::vector<std::unique_ptr<StylePropertyNode>>& getProperties() const;
    void print(int indent = 0) const override;

private:
    std::string selector;
    std::vector<std::unique_ptr<StylePropertyNode>> properties;
};

} // namespace CHTL

#endif //CHTL_CSSRULENODE_H