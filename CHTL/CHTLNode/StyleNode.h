#ifndef CHTL_STYLE_NODE_H
#define CHTL_STYLE_NODE_H

#include "BaseNode.h"
#include "INodeVisitor.h"
#include <string>

namespace CHTL
{
    namespace AST
    {
        class StyleNode : public BaseNode
        {
        public:
            // For now, we store the raw content. Parsing this is a later step.
            std::string rawContent;

            StyleNode(const std::string& content) : rawContent(content) {}

            void Accept(INodeVisitor& visitor) override
            {
                visitor.Visit(*this);
            }
        };
    }
}

#endif // CHTL_STYLE_NODE_H
