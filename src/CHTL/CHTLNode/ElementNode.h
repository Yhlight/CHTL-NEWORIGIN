/**
 * @file ElementNode.h
 * @brief Element node representing HTML elements
 */

#ifndef CHTL_ELEMENT_NODE_H
#define CHTL_ELEMENT_NODE_H

#include "BaseNode.h"

namespace CHTL {

/**
 * @class ElementNode
 * @brief Represents an HTML element
 */
class ElementNode : public BaseNode {
public:
    explicit ElementNode(const std::string& tagName);
    
    // Tag name
    void setTagName(const std::string& name) { tagName_ = name; }
    std::string getTagName() const { return tagName_; }
    
    // Self-closing tags
    void setSelfClosing(bool selfClosing) { selfClosing_ = selfClosing; }
    bool isSelfClosing() const { return selfClosing_; }
    
    // Code generation
    std::string toHTML() const override;
    
    // Visitor pattern
    void accept(NodeVisitor& visitor) override;
    
    // Debug
    std::string toString() const override;

private:
    std::string tagName_;
    bool selfClosing_;
    
    // Helper methods
    std::string attributesToString() const;
    bool isSelfClosingTag(const std::string& tag) const;
};

} // namespace CHTL

#endif // CHTL_ELEMENT_NODE_H
