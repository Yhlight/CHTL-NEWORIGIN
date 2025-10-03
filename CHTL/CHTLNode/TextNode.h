#ifndef CHTL_TEXT_NODE_H
#define CHTL_TEXT_NODE_H

#include "BaseNode.h"
#include "NodeVisitor.h"
#include <string>
#include <sstream>

class TextNode : public BaseNode {
public:
    explicit TextNode(const std::string& text) : text(text) {}

    std::string toString(int depth = 0) const override {
        std::stringstream ss;
        ss << std::string(depth * 2, ' ') << text << std::endl;
        return ss.str();
    }

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<TextNode>(text);
    }

    const std::string& getText() const { return text; }

private:
    std::string text;
};

#endif //CHTL_TEXT_NODE_H