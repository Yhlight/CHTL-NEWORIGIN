#ifndef TEMPLATE_NODE_H
#define TEMPLATE_NODE_H

#include "BaseNode.h"
#include "ElementNode.h" // For Attribute
#include <string>
#include <vector>

namespace CHTL {

enum class TemplateType {
    STYLE,
    ELEMENT,
    VAR
};

// This node represents a [Template] declaration in the source
class TemplateDeclarationNode : public BaseNode {
public:
    TemplateType template_type;
    std::string name;

    // For now, we only support style templates, which contain properties.
    // This will need to be more generic later (e.g., using std::any or a visitor)
    // to support element templates.
    std::vector<Attribute> style_properties;
};


} // namespace CHTL

#endif // TEMPLATE_NODE_H
