#pragma once

#include "BaseNode.h"
#include "../CHTLExpr/BaseExprNode.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>

// Represents a [Configuration] block
class ConfigNode : public BaseNode {
public:
    // Using BaseExprNode to allow for future evaluation of complex values
    std::map<std::string, std::shared_ptr<BaseExprNode>> settings;

    ConfigNode() : BaseNode(NodeType::Configuration) {}

    void print(int level = 0) const override {
        std::cout << std::string(level * 2, ' ') << "ConfigNode {\n";
        for (const auto& pair : settings) {
            std::cout << std::string((level + 1) * 2, ' ') << pair.first << " = ...\n";
        }
        std::cout << std::string(level * 2, ' ') << "}\n";
    }
};
