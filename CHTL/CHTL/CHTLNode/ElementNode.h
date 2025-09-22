#ifndef CHTL_ELEMENT_NODE_H
#define CHTL_ELEMENT_NODE_H

#include "BaseNode.h"
#include "Attribute.h"
#include "../CHTLGenerator/Generator.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>

namespace CHTL {

class ElementNode : public BaseNode {
public:
    std::string tagName;
    std::vector<Attribute> attributes;
    std::vector<std::unique_ptr<BaseNode>> children;

    std::string toString() const override {
        std::stringstream ss;
        ss << "ElementNode { Tag: " << tagName;
        if (!attributes.empty()) {
            ss << ", Attributes: [";
            for (const auto& attr : attributes) {
                ss << " " << attr.key << "=\"" << attr.value << "\"";
            }
            ss << " ]";
        }
        ss << ", Children: [\n";
        for (const auto& child : children) {
            if (child) {
                ss << child->toString() << "\n";
            }
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
