#pragma once

namespace CHTL
{
    class AstVisitor; // Forward declaration for the Visitor pattern

    // This is the abstract base class for all nodes in the Abstract Syntax Tree.
    class BaseNode
    {
    public:
        virtual ~BaseNode() = default;

        // The 'accept' method is part of the Visitor pattern. It allows
        // different operations (like code generation) to be performed on nodes
        // without changing the node classes themselves.
        virtual void accept(AstVisitor &visitor) = 0;
    };
}
