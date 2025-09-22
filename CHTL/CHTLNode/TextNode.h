#pragma once

#include "Node.h"

class TextNode : public Node {
public:
    TextNode(const std::string& content) : content(content) {}

    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }

    std::string getContent() const { return content; }

private:
    std::string content;
};
