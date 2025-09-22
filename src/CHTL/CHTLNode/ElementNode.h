#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>
#include <iostream>

namespace CHTL {

    class ElementNode : public BaseNode {
    public:
        ElementNode(const std::string& tagName);

        void print(int indent = 0) const override;
        const std::string& getTagName() const;
        void addChild(std::unique_ptr<BaseNode> child);

    private:
        std::string tagName;
        std::vector<std::unique_ptr<BaseNode>> children;
    };

} // namespace CHTL
