#pragma once

#include "BaseNode.h"
#include <string>
#include <iostream>

namespace CHTL {
    class CommentNode : public BaseNode {
    public:
        CommentNode(const std::string& text);
        void print(int indent = 0) const override;
    private:
        std::string text;
    };
}
