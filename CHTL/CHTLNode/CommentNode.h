#pragma once

#include "BaseNode.h"
#include <string>

class CommentNode : public BaseNode {
public:
    std::string content;
    bool isGeneratorComment;

    CommentNode(std::string content, bool isGenerator = false)
        : content(std::move(content)), isGeneratorComment(isGenerator) {}

    void accept(AstVisitor& visitor) override {
        visitor.visit(*this);
    }
};
