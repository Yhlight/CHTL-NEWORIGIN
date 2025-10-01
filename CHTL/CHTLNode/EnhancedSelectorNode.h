#ifndef CHTL_ENHANCED_SELECTOR_NODE_H
#define CHTL_ENHANCED_SELECTOR_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class EnhancedSelectorNode : public BaseNode {
public:
    explicit EnhancedSelectorNode(const std::string& selector);
    void print(int level = 0) const override;
    const std::string& getSelector() const;

private:
    std::string m_selector;
};

} // namespace CHTL

#endif // CHTL_ENHANCED_SELECTOR_NODE_H