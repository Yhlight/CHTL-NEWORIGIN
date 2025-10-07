#ifndef CHTL_COMMENT_NODE_H
#define CHTL_COMMENT_NODE_H

#include "../BaseNode.h"

namespace CHTL {

class CommentNode : public BaseNode {
public:
    enum class CommentType {
        Line,        // //
        Block,       // /* */
        Generator,   // #
    };

    CommentNode(const String& content, CommentType commentType,
                const SourceRange& range = SourceRange());

    const String& getContent() const { return content_; }
    CommentType getCommentType() const { return commentType_; }

    void accept(NodeVisitor& visitor) override;
    String toString(int indent = 0) const override;
    SharedPtr<BaseNode> clone() const override;

private:
    String content_;
    CommentType commentType_;
};

} // namespace CHTL

#endif // CHTL_COMMENT_NODE_H
