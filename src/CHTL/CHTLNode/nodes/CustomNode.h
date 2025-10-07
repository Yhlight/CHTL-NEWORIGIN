#ifndef CHTL_CUSTOM_NODE_H
#define CHTL_CUSTOM_NODE_H

#include "../BaseNode.h"

namespace CHTL {

class CustomNode : public BaseNode {
public:
    enum class CustomType {
        Style,      // @Style
        Element,    // @Element
        Var,        // @Var
    };

    CustomNode(CustomType customType, const String& name,
               const SourceRange& range = SourceRange());

    CustomType getCustomType() const { return customType_; }
    const String& getName() const { return name_; }

    void accept(NodeVisitor& visitor) override;
    String toString(int indent = 0) const override;
    SharedPtr<BaseNode> clone() const override;

private:
    CustomType customType_;
    String name_;
};

} // namespace CHTL

#endif // CHTL_CUSTOM_NODE_H
