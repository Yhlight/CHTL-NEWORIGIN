#ifndef CHTL_STYLE_NODE_H
#define CHTL_STYLE_NODE_H

#include "../BaseNode.h"

namespace CHTL {

class StyleNode : public BaseNode {
public:
    StyleNode(const SourceRange& range = SourceRange());

    // 内联样式
    void addInlineStyle(const String& property, const String& value);
    const HashMap<String, String>& getInlineStyles() const { return inlineStyles_; }

    // CSS规则
    void addCssRule(const String& selector, const String& rules);
    const HashMap<String, String>& getCssRules() const { return cssRules_; }

    void accept(NodeVisitor& visitor) override;
    String toString(int indent = 0) const override;
    SharedPtr<BaseNode> clone() const override;

private:
    HashMap<String, String> inlineStyles_;  // 内联样式
    HashMap<String, String> cssRules_;      // CSS规则
};

} // namespace CHTL

#endif // CHTL_STYLE_NODE_H
