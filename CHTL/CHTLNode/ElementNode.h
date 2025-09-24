#ifndef CHTL_ELEMENT_NODE_H
#define CHTL_ELEMENT_NODE_H

#include "BaseNode.h"
#include "AttributeNode.h"
#include "INodeVisitor.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL
{
    namespace AST
    {
        class ElementNode : public BaseNode
        {
        public:
            std::string tagName;
            std::vector<std::unique_ptr<AttributeNode>> attributes;
            std::vector<std::unique_ptr<BaseNode>> children;

            ElementNode(const std::string& name) : tagName(name) {}

            void Accept(INodeVisitor& visitor) override
            {
                visitor.Visit(*this);
            }
        };
    }
}

#endif // CHTL_ELEMENT_NODE_H
