#pragma once

#include "BaseNode.h"
#include <string>
#include <map>
#include <vector>
#include <sstream>

namespace CHTL {

// Represents a single CSS rule like ".class { ... }" or a set of inline properties
struct StyleRule {
    std::string selector; // e.g., ".box", "#main", "&:hover". Empty for inline styles.
    std::map<std::string, std::string> properties;
};

class StyleNode : public BaseNode {
public:
    std::vector<StyleRule> rules;
    std::vector<std::shared_ptr<BaseNode>> children; // For @Style usage

    StyleNode() {}

    NodeType getType() const override { return NodeType::StyleBlock; }

    // StyleNode does not generate output directly. It modifies its parent element.
    // So toString can be simple for debugging.
    std::string toString(int indentation = 0) const override {
        std::stringstream ss;
        std::string indent(indentation, ' ');
        ss << indent << "[StyleNode]" << std::endl;
        for (const auto& rule : rules) {
            ss << indent << "  Selector: " << (rule.selector.empty() ? "(inline)" : rule.selector) << std::endl;
            for (const auto& prop : rule.properties) {
                ss << indent << "    " << prop.first << ": " << prop.second << ";" << std::endl;
            }
        }
        return ss.str();
    }
};

} // namespace CHTL
