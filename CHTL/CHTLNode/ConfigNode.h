#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <map>
#include <iostream>

class ConfigNode : public BaseNode {
public:
    // General configuration settings
    std::map<std::string, std::string> settings;

    // For the [Name] block specifically
    std::map<std::string, std::vector<std::string>> nameMappings;

    ConfigNode() = default;

    virtual void print(int level = 0) const override {
        for (int i = 0; i < level; ++i) std::cout << "  ";
        std::cout << "ConfigNode" << std::endl;
        for (const auto& pair : settings) {
            for (int i = 0; i < level + 1; ++i) std::cout << "  ";
            std::cout << pair.first << " = " << pair.second << std::endl;
        }
        if (!nameMappings.empty()) {
            for (int i = 0; i < level + 1; ++i) std::cout << "  ";
            std::cout << "[Name]" << std::endl;
            for (const auto& pair : nameMappings) {
                for (int i = 0; i < level + 2; ++i) std::cout << "  ";
                std::cout << pair.first << " = [ ";
                for(const auto& val : pair.second) std::cout << val << " ";
                std::cout << "]" << std::endl;
            }
        }
    }
};
