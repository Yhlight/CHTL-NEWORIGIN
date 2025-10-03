#pragma once

#include "BaseNode.h"
#include <string>

class CommentNode : public BaseNode {
public:
    CommentNode(const std::string& comment) : comment(comment) {}

    NodeType getType() const override { return NodeType::Comment; }

    const std::string& getComment() const {
        return comment;
    }

private:
    std::string comment;
};