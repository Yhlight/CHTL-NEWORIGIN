#pragma once

#include "CHTL/CHTLNode/BaseNode.h"

namespace CHTL {

class ElementNode : public BaseNode {
public:
    ElementNode(const std::string& tagName);
    ~ElementNode() override = default;

    bool isValid() const override;
    std::string getValidationError() const override;
    std::string generateHTML() const override;
    std::string generateCSS() const override;
    std::string generateJS() const override;
    std::unique_ptr<BaseNode> clone() const override;

    // Element-specific methods
    void setTagName(const std::string& tagName);
    const std::string& getTagName() const;
    void setSelfClosing(bool selfClosing);
    bool isSelfClosing() const;

private:
    std::string tagName_;
    bool selfClosing_;
};

} // namespace CHTL