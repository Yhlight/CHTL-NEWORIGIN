#pragma once

#include "BaseNode.h"
#include <vector>
#include <memory>
#include <iostream>

namespace CHTL {

    class RootNode : public BaseNode {
    public:
        std::vector<std::unique_ptr<BaseNode>> children;

        void print(int indent = 0) const override {
            // Create indentation string
            std::string indentStr;
            for (int i = 0; i < indent; ++i) {
                indentStr += "  ";
            }
            std::cout << indentStr << "RootNode:" << std::endl;
            for(const auto& child : children) {
                if (child) {
                    child->print(indent + 1);
                }
            }
        }
    };
}
