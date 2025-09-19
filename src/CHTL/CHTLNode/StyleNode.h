#ifndef STYLE_NODE_H
#define STYLE_NODE_H

#include "CHTL/CHTLNode/BaseNode.h"
#include "CHTL/CHTLNode/ElementNode.h" // For Attribute struct
#include <vector>
#include <string>

namespace CHTL {

struct RuleNode {
    std::string selector;
    std::vector<Attribute> properties;
};

class StyleNode : public BaseNode {
public:
    // Properties that will be applied as inline styles
    std::vector<Attribute> inline_properties;
    // Rules that will be added to a global stylesheet
    std::vector<RuleNode> rules;
};

} // namespace CHTL

#endif // STYLE_NODE_H
