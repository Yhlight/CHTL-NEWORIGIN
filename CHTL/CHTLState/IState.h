#ifndef CHTL_ISTATE_H
#define CHTL_ISTATE_H

#include <memory>

namespace CHTL
{
    // Forward declaration to break circular dependency
    class Parser;

    namespace State
    {
        class IState
        {
        public:
            virtual ~IState() = default;

            // Handle is now void, as the AST is managed on the parser's node stack.
            virtual void Handle(Parser& parser) = 0;
        };
    }
}

#endif // CHTL_ISTATE_H
