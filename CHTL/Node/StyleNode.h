#pragma once

#include "BaseNode.h"
#include <vector>
#include <string>
#include <utility>
#include <sstream>
#include <memory>

namespace CHTL {

struct StyleRule {
    std::string selector;
    std::vector<std::pair<std::string, std::string>> properties;
};

class StyleNode : public BaseNode {
public:
    std::vector<std::pair<std::string, std::string>> inline_properties;
    std::vector<StyleRule> rules;
    std::vector<std::unique_ptr<BaseNode>> children; // To hold @Style usages

    NodeType getType() const override {
        return NodeType::Style;
    }

    void addInlineProperty(const std::string& key, const std::string& value) {
        inline_properties.push_back({key, value});
    }

    void addRule(const StyleRule& rule) {
        rules.push_back(rule);
    }

    std::string toString(int indent = 0) const override {
        std::string ind(indent * 2, ' ');
        std::stringstream ss;
        ss << ind << "StyleNode {\n";
        for (const auto& prop : inline_properties) {
            ss << ind << "  " << prop.first << ": " << prop.second << ";\n";
        }
        for (const auto& rule : rules) {
            ss << ind << "  " << rule.selector << " {\n";
            for (const auto& prop : rule.properties) {
                ss << ind << "    " << prop.first << ": " << prop.second << ";\n";
            }
            ss << ind << "  }\n";
        }
        ss << ind << "}\n";
        return ss.str();
    }
};

} // namespace CHTL
