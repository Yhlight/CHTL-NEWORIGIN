#pragma once

namespace CHTL {

// A generic strategy interface. The template parameter allows strategies
// to operate on different types of context objects (e.g., parsing context,
// generation context).
template<typename TContext>
class IStrategy {
public:
    virtual ~IStrategy() = default;
    virtual void execute(TContext& context) = 0;
};

} // namespace CHTL
