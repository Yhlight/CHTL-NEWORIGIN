#pragma once

#include "BaseNode.h"
#include <string>

class CommentNode : public BaseNode {
public:
    CommentNode(const std::string& content) : content(content) {}

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::string content;
};