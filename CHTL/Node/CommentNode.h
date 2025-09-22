#pragma once

#include "Node.h"
#include <string>
#include <sstream>
#include "CHTL/AstVisitor.h"

class CommentNode : public Node {
public:
    explicit CommentNode(std::string comment) : comment(std::move(comment)) {}

    void accept(AstVisitor& visitor) const override {
        visitor.visit(*this);
    }

    std::string to_string(int indent = 0) const override {
        std::stringstream ss;
        std::string indentation(indent * 2, ' ');
        ss << indentation << "COMMENT: " << comment << std::endl;
        return ss.str();
    }

    const std::string comment;
};
