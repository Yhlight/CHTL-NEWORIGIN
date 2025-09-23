#pragma once

#include "BaseNode.h"
#include <string>

struct CommentNode : public BaseNode {
    std::string content;
    bool isGeneratorAware;

    CommentNode(const std::string& content, bool isGeneratorAware)
        : content(content), isGeneratorAware(isGeneratorAware) {}

    NodeType getType() const override { return NodeType::Comment; }

    void print(int level = 0) const override {
        for (int i = 0; i < level; ++i) std::cout << "  ";
        std::cout << "CommentNode (" << (isGeneratorAware ? "#" : "//") << content << ")" << std::endl;
    }
};
