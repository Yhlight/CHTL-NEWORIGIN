#pragma once

#include "BaseNode.h"
#include "StylePropertyNode.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>

namespace CHTL {

/**
 * @class SelectorBlockNode
 * @brief Represents a nested selector block within a style block (e.g., .my-class { ... }).
 */
class SelectorBlockNode : public BaseNode {
public:
    explicit SelectorBlockNode(const std::string& selector) : selector(selector) {}

    std::string toString(int indent = 0) const override {
        std::stringstream ss;
        std::string indentation(indent, ' ');
        ss << indentation << "SelectorBlock(" << selector << ") [\n";

        for (const auto& prop : properties) {
            ss << prop->toString(indent + 2) << "\n";
        }

        ss << indentation << "]";
        return ss.str();
    }

    std::string selector;
    std::vector<std::unique_ptr<StylePropertyNode>> properties;
};

} // namespace CHTL
