#ifndef CHTL_TEXT_NODE_H
#define CHTL_TEXT_NODE_H

#include "../BaseNode.h"

namespace CHTL {

class TextNode : public BaseNode {
public:
    TextNode(const String& content, const SourceRange& range = SourceRange());

    const String& getContent() const { return content_; }
    void setContent(const String& content) { content_ = content; }

    void accept(NodeVisitor& visitor) override;
    String toString(int indent = 0) const override;
    SharedPtr<BaseNode> clone() const override;

private:
    String content_;
};

} // namespace CHTL

#endif // CHTL_TEXT_NODE_H
