#ifndef CHTL_COMMENT_NODE_H
#define CHTL_COMMENT_NODE_H

#include "Node.h"
#include <string>

class CommentNode : public Node {
public:
    explicit CommentNode(const std::string& comment) : comment(comment) {}

    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "CommentNode: " + comment;
    }

    const std::string& getComment() const { return comment; }

private:
    std::string comment;
};

#endif // CHTL_COMMENT_NODE_H