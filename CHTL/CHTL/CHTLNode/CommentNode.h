#ifndef CHTL_COMMENT_NODE_H
#define CHTL_COMMENT_NODE_H

#include "BaseNode.h"
#include "Visitor.h"
#include <string>

// Represents a generator comment, like '# this is a comment'.
// These comments are preserved in the AST and rendered into the output.
class CommentNode : public BaseNode {
public:
    CommentNode(const std::string& content) : content(content) {}

    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }

    const std::string& getContent() const { return content; }

private:
    std::string content;
};

#endif // CHTL_COMMENT_NODE_H
