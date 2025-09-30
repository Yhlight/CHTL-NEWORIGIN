#ifndef CHTL_STYLE_NODE_H
#define CHTL_STYLE_NODE_H

#include "BaseNode.h"
#include "StylePropertyNode.h"
#include <vector>
#include <memory>

namespace CHTL {

class StyleNode : public BaseNode {
public:
    const std::vector<std::unique_ptr<StylePropertyNode>>& getProperties() const { return properties; }

    void addProperty(std::unique_ptr<StylePropertyNode> property) {
        properties.push_back(std::move(property));
    }

private:
    std::vector<std::unique_ptr<StylePropertyNode>> properties;
};

} // namespace CHTL

#endif // CHTL_STYLE_NODE_H