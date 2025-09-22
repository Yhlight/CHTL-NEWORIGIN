#ifndef CHTL_TEMPLATE_STYLE_NODE_H
#define CHTL_TEMPLATE_STYLE_NODE_H

#include "BaseNode.h"
#include "StyleProperty.h"
#include "../CHTLGenerator/Generator.h"
#include <string>
#include <vector>
#include <sstream>

namespace CHTL {

class TemplateStyleNode : public BaseNode {
public:
    std::string name;
    std::vector<StyleProperty> properties;

    std::string toString() const override {
        std::stringstream ss;
        ss << "TemplateStyleNode { Name: " << name << ", Properties: [";
        for (const auto& prop : properties) {
            ss << " " << prop.key << ":" << prop.value << ";";
        }
        ss << " ] }";
        return ss.str();
    }

    void accept(Generator& visitor) const override {
        visitor.visit(*this);
    }
};

} // namespace CHTL

#endif // CHTL_TEMPLATE_STYLE_NODE_H
