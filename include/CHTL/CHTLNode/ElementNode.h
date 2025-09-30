#pragma once
#include "CHTL/CHTLNode/BaseNode.h"

namespace CHTL {

class ElementNode : public BaseNode {
public:
    ElementNode(const std::string& tagName = "");
    virtual ~ElementNode() = default;
    
    std::string toHtml() const override;
    std::string toCss() const override;
    std::string toJs() const override;
    bool isValid() const override;
};

} // namespace CHTL