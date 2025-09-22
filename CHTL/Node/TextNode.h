#pragma once

#include "Node.h"
#include <string>
#include <sstream>
#include "CHTL/AstVisitor.h"

class TextNode : public Node {
public:
    explicit TextNode(std::string text) : text(std::move(text)) {}

    void accept(AstVisitor& visitor) const override {
        visitor.visit(*this);
    }

    std::string to_string(int indent = 0) const override {
        std::stringstream ss;
        std::string indentation(indent * 2, ' ');
        ss << indentation << "TEXT: \"" << text << "\"" << std::endl;
        return ss.str();
    }

    const std::string text;
};
