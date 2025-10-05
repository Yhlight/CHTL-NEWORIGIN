#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

    class TextNode : public BaseNode {
    public:
        TextNode(const std::string& content) : BaseNode(NodeType::NODE_TEXT), content(content) {}

        const std::string& getContent() const { return content; }

    private:
        std::string content;
    };

}