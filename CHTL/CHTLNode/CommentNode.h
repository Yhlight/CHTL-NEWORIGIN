#ifndef CHTL_COMMENT_NODE_H
#define CHTL_COMMENT_NODE_H

#include "Node.h"
#include <string>

namespace CHTL {

class CommentNode : public Node {
public:
    explicit CommentNode(const std::string& text);

    void Accept(AstVisitor& visitor) override;

    std::string text;
};

} // namespace CHTL

#endif // CHTL_COMMENT_NODE_H
