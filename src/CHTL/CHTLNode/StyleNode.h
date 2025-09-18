#pragma once

#include "BaseNode.h"
#include "AstVisitor.h"
#include <string>
#include <map>

namespace CHTL
{
    // Represents a local style block, e.g., style { color: red; }
    // This node will be interpreted by the generator to create an inline
    // style attribute on its parent element.
    class StyleNode : public BaseNode
    {
    public:
        std::map<std::string, std::string> properties;

        void addProperty(const std::string& key, const std::string& value) {
            properties[key] = value;
        }

        void accept(AstVisitor &visitor) override
        {
            visitor.visit(*this);
        }
    };
}
