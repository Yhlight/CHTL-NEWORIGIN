#ifndef CHTL_COMMENT_NODE_H
#define CHTL_COMMENT_NODE_H

#include "BaseNode.h"
#include "INodeVisitor.h"
#include <string>

namespace CHTL
{
    namespace AST
    {
        class CommentNode : public BaseNode
        {
        public:
            std::string comment;
            // We might need a type to distinguish between //, /*...*/, and #
            // For now, a single class is sufficient.

            CommentNode(const std::string& comment) : comment(comment) {}

            void Accept(INodeVisitor& visitor) override
            {
                visitor.Visit(*this);
            }
        };
    }
}

#endif // CHTL_COMMENT_NODE_H
