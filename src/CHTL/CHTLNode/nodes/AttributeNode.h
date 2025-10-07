#ifndef CHTL_ATTRIBUTE_NODE_H
#define CHTL_ATTRIBUTE_NODE_H

#include "../BaseNode.h"

namespace CHTL {

class AttributeNode : public BaseNode {
public:
    AttributeNode(const String& name, const String& value,
                  const SourceRange& range = SourceRange());

    const String& getName() const { return name_; }
    const String& getValue() const { return value_; }

    void setName(const String& name) { name_ = name; }
    void setValue(const String& value) { value_ = value; }

    void accept(NodeVisitor& visitor) override;
    String toString(int indent = 0) const override;
    SharedPtr<BaseNode> clone() const override;

private:
    String name_;
    String value_;
};

} // namespace CHTL

#endif // CHTL_ATTRIBUTE_NODE_H
