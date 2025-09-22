#pragma once

#include "BaseNode.h"
#include <string>
#include <iostream>

namespace CHTL {

    class TextNode : public BaseNode {
    public:
        TextNode(const std::string& text);

        void print(int indent = 0) const override;
        const std::string& getText() const;

    private:
        std::string text;
    };

} // namespace CHTL
