#ifndef CHTL_BASE_NODE_H
#define CHTL_BASE_NODE_H

#include <memory>
#include <vector>

namespace CHTL
{
    namespace AST
    {
        // Forward declaration for the visitor pattern
        class INodeVisitor;

        class BaseNode
        {
        public:
            virtual ~BaseNode() = default;
            virtual void Accept(INodeVisitor& visitor) = 0;
        };
    }
}

#endif // CHTL_BASE_NODE_H
