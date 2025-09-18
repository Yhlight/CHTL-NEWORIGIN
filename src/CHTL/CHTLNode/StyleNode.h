#pragma once

#include "BaseNode.h"
#include "AstVisitor.h"
#include "./Expression/ExprNode.h"
#include <string>
#include <map>
#include <memory>

namespace CHTL
{
    // Represents a local style block, e.g., style { color: red; }
    // This node will be interpreted by the generator to create an inline
    // style attribute on its parent element.
    class StyleNode : public BaseNode
    {
    public:
        std::map<std::string, std::unique_ptr<ExprNode>> properties;

        void addProperty(const std::string& key, std::unique_ptr<ExprNode> value) {
            properties[key] = std::move(value);
        }

        void accept(AstVisitor &visitor) override
        {
            visitor.visit(*this);
        }
    };
}
