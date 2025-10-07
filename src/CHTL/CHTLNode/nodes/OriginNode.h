#ifndef CHTL_ORIGIN_NODE_H
#define CHTL_ORIGIN_NODE_H

#include "../BaseNode.h"

namespace CHTL {

class OriginNode : public BaseNode {
public:
    enum class OriginType {
        Html,
        Style,
        JavaScript,
        Custom,  // 自定义类型
    };

    OriginNode(OriginType originType, const String& content,
               const SourceRange& range = SourceRange());

    OriginType getOriginType() const { return originType_; }
    const String& getContent() const { return content_; }
    const String& getTypeName() const { return typeName_; }

    void setTypeName(const String& name) { typeName_ = name; }

    void accept(NodeVisitor& visitor) override;
    String toString(int indent = 0) const override;
    SharedPtr<BaseNode> clone() const override;

private:
    OriginType originType_;
    String content_;
    String typeName_;  // 对于自定义类型，存储类型名称
};

} // namespace CHTL

#endif // CHTL_ORIGIN_NODE_H
