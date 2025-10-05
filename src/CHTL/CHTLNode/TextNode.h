#ifndef CHTL_TEXT_NODE_H
#define CHTL_TEXT_NODE_H

#include "Node.h"
#include <string>

class TextNode : public Node {
public:
    explicit TextNode(const std::string& text) : text(text) {}

    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "TextNode: " + text;
    }

    const std::string& getText() const { return text; }

private:
    std::string text;
};

#endif // CHTL_TEXT_NODE_H