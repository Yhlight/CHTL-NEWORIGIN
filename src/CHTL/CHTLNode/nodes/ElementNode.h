#ifndef CHTL_ELEMENT_NODE_H
#define CHTL_ELEMENT_NODE_H

#include "../BaseNode.h"

namespace CHTL {

class ElementNode : public BaseNode {
public:
    ElementNode(const String& tagName, const SourceRange& range = SourceRange());

    const String& getTagName() const { return tagName_; }
    void setTagName(const String& name) { tagName_ = name; }

    // 属性管理
    void setAttribute(const String& name, const String& value);
    Optional<String> getAttribute(const String& name) const;
    const HashMap<String, String>& getAttributes() const { return attributes_; }
    bool hasAttribute(const String& name) const;

    void accept(NodeVisitor& visitor) override;
    String toString(int indent = 0) const override;
    SharedPtr<BaseNode> clone() const override;

private:
    String tagName_;
    HashMap<String, String> attributes_;
};

} // namespace CHTL

#endif // CHTL_ELEMENT_NODE_H
