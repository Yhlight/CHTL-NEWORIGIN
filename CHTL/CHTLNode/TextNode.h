#ifndef CHTL_TEXT_NODE_H
#define CHTL_TEXT_NODE_H

#include "BaseNode.h"
#include "INodeVisitor.h"
#include <string>

namespace CHTL
{
    namespace AST
    {
        class TextNode : public BaseNode
        {
        public:
            std::string content;

            TextNode(const std::string& content) : content(content) {}

            void Accept(INodeVisitor& visitor) override
            {
                visitor.Visit(*this);
            }
        };
    }
}

#endif // CHTL_TEXT_NODE_H
