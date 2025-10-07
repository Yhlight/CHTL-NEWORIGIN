#pragma once

#include "Node.h"
#include <string>

class CommentNode : public Node {
public:
    CommentNode(const std::string& content) : content(content) {}

    std::string content;

    NodePtr clone() const override {
        return std::make_shared<CommentNode>(content);
    }
};