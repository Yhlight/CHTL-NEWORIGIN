#ifndef SELECTOR_BLOCK_NODE_H
#define SELECTOR_BLOCK_NODE_H

#include "BaseNode.h"
#include "StylePropertyNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class SelectorBlockNode : public BaseNode {
public:
    explicit SelectorBlockNode(const std::string& selector);

    void addProperty(std::unique_ptr<StylePropertyNode> property);

    const std::string& getSelector() const;
    const std::vector<std::unique_ptr<StylePropertyNode>>& getProperties() const;

    void print(int indent = 0) const override;

private:
    std::string m_selector;
    std::vector<std::unique_ptr<StylePropertyNode>> m_properties;
};

} // namespace CHTL

#endif // SELECTOR_BLOCK_NODE_H