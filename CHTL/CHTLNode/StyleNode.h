#ifndef CHTL_STYLE_NODE_H
#define CHTL_STYLE_NODE_H

#include "BaseNode.h"
#include "INodeVisitor.h"
#include "CssPropertyNode.h"
#include <vector>
#include <memory>

namespace CHTL
{
    namespace AST
    {
        class StyleNode : public BaseNode
        {
        public:
            std::vector<std::unique_ptr<CssPropertyNode>> properties;

            StyleNode() = default;

            void Accept(INodeVisitor& visitor) override
            {
                visitor.Visit(*this);
            }
        };
    }
}

#endif // CHTL_STYLE_NODE_H
