#pragma once

#include "BaseNode.h"

// Represents a text block, e.g., `text { "content" }`
class TextNode : public BaseNode {
public:
    std::string content;

    explicit TextNode(std::string content) : content(std::move(content)) {}

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::shared_ptr<BaseNode> clone() const override {
        return std::make_shared<TextNode>(this->content);
    }
};
