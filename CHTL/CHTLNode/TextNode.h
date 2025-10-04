#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

    class TextNode : public BaseNode {
    public:
        TextNode(const std::string& content) : content(content) {
            type = NodeType::NODE_TEXT;
        }

        const std::string& getContent() const { return content; }

    private:
        std::string content;
    };

}