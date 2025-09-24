#ifndef CHTL_TEXT_NODE_H
#define CHTL_TEXT_NODE_H

#include "BaseNode.h"
#include "Visitor.h"
#include <string>

// Represents a text block in CHTL, like 'text { "some content" }'.
// It stores the string content of the text block.
class TextNode : public BaseNode {
public:
    TextNode(const std::string& content) : content(content) {}

    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }

    const std::string& getContent() const { return content; }

private:
    std::string content;
};

#endif // CHTL_TEXT_NODE_H
