#ifndef CHTL_ATTRIBUTE_NODE_H
#define CHTL_ATTRIBUTE_NODE_H

#include "BaseNode.h"
#include "INodeVisitor.h"
#include <string>

namespace CHTL
{
    namespace AST
    {
        class AttributeNode : public BaseNode
        {
        public:
            std::string name;
            std::string value; // For now, value is a simple string. This might evolve.

            AttributeNode(const std::string& name, const std::string& value)
                : name(name), value(value) {}

            void Accept(INodeVisitor& visitor) override
            {
                visitor.Visit(*this);
            }
        };
    }
}

#endif // CHTL_ATTRIBUTE_NODE_H
