#ifndef CHTL_CSS_PROPERTY_NODE_H
#define CHTL_CSS_PROPERTY_NODE_H

#include "BaseNode.h"
#include "INodeVisitor.h"
#include <string>

namespace CHTL
{
    namespace AST
    {
        class CssPropertyNode : public BaseNode
        {
        public:
            std::string property;
            std::string value;

            CssPropertyNode(const std::string& prop, const std::string& val)
                : property(prop), value(val) {}

            void Accept(INodeVisitor& visitor) override
            {
                visitor.Visit(*this);
            }
        };
    }
}

#endif // CHTL_CSS_PROPERTY_NODE_H
