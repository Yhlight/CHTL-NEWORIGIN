#pragma once

#include "BaseNode.h"

namespace CHTL {

class ElementNode : public BaseNode {
public:
    ElementNode(const std::string& name = "");
    
    // Element-specific properties
    std::string getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }
    
    // Attributes
    void setAttribute(const std::string& key, const std::string& value);
    std::string getAttribute(const std::string& key) const;
    bool hasAttribute(const std::string& key) const;
    void removeAttribute(const std::string& key);
    
    // Behaviors (style, script, text)
    void addBehavior(std::unique_ptr<BaseNode> behavior);
    std::vector<BaseNode*> getBehaviors() const;
    BaseNode* getBehavior(const std::string& type) const;
    
    // Element validation
    bool isValid() const override;
    std::vector<std::string> getValidationErrors() const override;
    
    // Visitor pattern
    void accept(NodeVisitor* visitor) override;
    
    // String representation
    std::string toString() const override;
    
private:
    std::string name_;
    std::map<std::string, std::string> attributes_;
    std::vector<std::unique_ptr<BaseNode>> behaviors_;
};

} // namespace CHTL