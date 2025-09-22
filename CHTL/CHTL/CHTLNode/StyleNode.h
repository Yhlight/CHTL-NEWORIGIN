#ifndef CHTL_STYLE_NODE_H
#define CHTL_STYLE_NODE_H

#include "BaseNode.h"
#include "StyleProperty.h"
#include "../CHTLGenerator/Generator.h"
#include <vector>
#include <sstream>

namespace CHTL {

class StyleNode : public BaseNode {
public:
    std::vector<StyleProperty> properties;

    std::string toString() const override {
        std::stringstream ss;
        ss << "StyleNode { Properties: [";
        for (const auto& prop : properties) {
            ss << " " << prop.key << ": " << prop.value << ";";
        }
        ss << " ] }";
        return ss.str();
    }

    void accept(Generator& visitor) const override {
        visitor.visit(*this);
    }
};

} // namespace CHTL

#endif // CHTL_STYLE_NODE_H
