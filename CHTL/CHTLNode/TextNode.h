#ifndef CHTL_TEXT_NODE_H
#define CHTL_TEXT_NODE_H

#include "BaseNode.h"
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

private:
    std::string text;
};

#endif //CHTL_TEXT_NODE_H