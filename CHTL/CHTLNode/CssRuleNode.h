#pragma once

#include "BaseNode.h"
#include "../CHTLExpr/BaseExprNode.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

// Represents a full CSS rule (e.g., .box { width: 100px; })
// to be stored in the context and evaluated later.
struct CssRuleNode : public BaseNode {
    std::string selector;
    std::map<std::string, std::shared_ptr<BaseExprNode>> properties;

    void print(int level = 0) const override {
        for (int i = 0; i < level; ++i) std::cout << "  ";
        std::cout << "CssRuleNode (" << selector << ")" << std::endl;
        for (const auto& prop : properties) {
            for (int i = 0; i < level + 1; ++i) std::cout << "  ";
            // This won't print the expression tree, but it's better than nothing
            std::cout << prop.first << ": [Expression]" << std::endl;
        }
    }
};
