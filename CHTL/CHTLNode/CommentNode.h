#ifndef CHTL_COMMENT_NODE_H
#define CHTL_COMMENT_NODE_H

#include "BaseNode.h"
#include "NodeVisitor.h"
#include <string>
#include <sstream>

class CommentNode : public BaseNode {
public:
    explicit CommentNode(const std::string& comment) : comment(comment) {}

    std::string toString(int depth = 0) const override {
        std::stringstream ss;
        ss << std::string(depth * 2, ' ') << "<!-- " << comment << " -->" << std::endl;
        return ss.str();
    }

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<CommentNode>(comment);
    }

    const std::string& getComment() const { return comment; }

private:
    std::string comment;
};

#endif //CHTL_COMMENT_NODE_H