#ifndef CHTL_COMMENT_NODE_H
#define CHTL_COMMENT_NODE_H

#include "CHTLNode.h"
#include <string>

// Forward declaration
class AstVisitor;

class CommentNode : public CHTLNode {
public:
    explicit CommentNode(const std::string& content);

    void accept(AstVisitor& visitor) override;

    const std::string& getContent() const;

private:
    std::string content;
};

#endif // CHTL_COMMENT_NODE_H
