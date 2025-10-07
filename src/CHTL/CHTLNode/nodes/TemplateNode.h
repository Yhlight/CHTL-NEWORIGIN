#ifndef CHTL_TEMPLATE_NODE_H
#define CHTL_TEMPLATE_NODE_H

#include "../BaseNode.h"

namespace CHTL {

class TemplateNode : public BaseNode {
public:
    enum class TemplateType {
        Style,      // @Style
        Element,    // @Element
        Var,        // @Var
    };

    TemplateNode(TemplateType templateType, const String& name,
                 const SourceRange& range = SourceRange());

    TemplateType getTemplateType() const { return templateType_; }
    const String& getName() const { return name_; }

    void accept(NodeVisitor& visitor) override;
    String toString(int indent = 0) const override;
    SharedPtr<BaseNode> clone() const override;

private:
    TemplateType templateType_;
    String name_;
};

} // namespace CHTL

#endif // CHTL_TEMPLATE_NODE_H
