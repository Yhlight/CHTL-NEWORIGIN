#ifndef CHTL_ISTRATEGY_H
#define CHTL_ISTRATEGY_H

namespace CHTL
{
    namespace Strategy
    {
        // A generic strategy interface.
        // The context type can be templated for more flexibility if needed,
        // but for now we can keep it simple.
        template <typename TContext>
        class IStrategy
        {
        public:
            virtual ~IStrategy() = default;

            // Execute the strategy with a given context.
            virtual void Execute(TContext& context) = 0;
        };
    }
}

#endif // CHTL_ISTRATEGY_H
