#include "CommentNode.h"
#include "../BaseNode.h"
#include <sstream>

namespace CHTL {

CommentNode::CommentNode(const String& content, CommentType commentType,
                         const SourceRange& range)
    : BaseNode(NodeType::Comment, range)
    , content_(content)
    , commentType_(commentType) {}

void CommentNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

String CommentNode::toString(int indent) const {
    std::stringstream ss;
    ss << indentString(indent) << "Comment(";
    switch (commentType_) {
        case CommentType::Line: ss << "Line"; break;
        case CommentType::Block: ss << "Block"; break;
        case CommentType::Generator: ss << "Generator"; break;
    }
    ss << ", \"" << content_ << "\")";
    return ss.str();
}

SharedPtr<BaseNode> CommentNode::clone() const {
    return std::make_shared<CommentNode>(content_, commentType_, range_);
}

} // namespace CHTL
