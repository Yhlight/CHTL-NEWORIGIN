#ifndef CHTL_SCRIPT_NODE_H
#define CHTL_SCRIPT_NODE_H

#include "../BaseNode.h"

namespace CHTL {

class ScriptNode : public BaseNode {
public:
    ScriptNode(const String& content, const SourceRange& range = SourceRange());

    const String& getContent() const { return content_; }
    void setContent(const String& content) { content_ = content; }

    bool isLocal() const { return isLocal_; }
    void setLocal(bool local) { isLocal_ = local; }

    void accept(NodeVisitor& visitor) override;
    String toString(int indent = 0) const override;
    SharedPtr<BaseNode> clone() const override;

private:
    String content_;
    bool isLocal_ = false;  // 是否是局部script块
};

} // namespace CHTL

#endif // CHTL_SCRIPT_NODE_H
