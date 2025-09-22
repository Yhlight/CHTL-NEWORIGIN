#ifndef CHTL_ELEMENT_NODE_H
#define CHTL_ELEMENT_NODE_H

#include "BaseNode.h"
#include "../CHTLGenerator/Generator.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>

namespace CHTL {

class ElementNode : public BaseNode {
public:
    std::string tagName;
    // Attributes will be added later as a separate node type or map
    std::vector<std::unique_ptr<BaseNode>> children;

    std::string toString() const override {
        std::stringstream ss;
        ss << "ElementNode { Tag: " << tagName << ", Children: [\n";
        for (const auto& child : children) {
            ss << child->toString() << "\n";
        }
        ss << "] }";
        return ss.str();
    }

    void accept(Generator& visitor) const override {
        visitor.visit(*this);
    }
};

} // namespace CHTL

#endif // CHTL_ELEMENT_NODE_H
