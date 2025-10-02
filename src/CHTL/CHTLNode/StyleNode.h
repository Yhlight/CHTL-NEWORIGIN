/**
 * @file StyleNode.h
 * @brief Style node for CSS
 */

#ifndef CHTL_STYLE_NODE_H
#define CHTL_STYLE_NODE_H

#include "BaseNode.h"
#include <unordered_map>

namespace CHTL {

class StyleNode : public BaseNode {
public:
    StyleNode();
    
    // Style properties
    void addProperty(const std::string& name, const std::string& value);
    std::string getProperty(const std::string& name) const;
    const std::unordered_map<std::string, std::string>& getProperties() const { return properties_; }
    
    // Inline vs global
    void setInline(bool isInline) { inline_ = isInline; }
    bool isInline() const { return inline_; }
    
    // Selector (for non-inline styles)
    void setSelector(const std::string& selector) { selector_ = selector; }
    std::string getSelector() const { return selector_; }
    
    std::string toHTML() const override;
    std::string toCSS() const override;
    void accept(NodeVisitor& visitor) override;
    std::string toString() const override;

private:
    std::unordered_map<std::string, std::string> properties_;
    bool inline_;
    std::string selector_;
};

} // namespace CHTL

#endif // CHTL_STYLE_NODE_H
