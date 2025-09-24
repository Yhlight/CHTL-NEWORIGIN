#pragma once

#include <memory>

namespace CHTL {

// Forward declaration of the context to avoid circular dependencies
class CHTLContext;

/**
 * @class Strategy
 * @brief An abstract base class representing a strategy in the Strategy Pattern.
 *
 * Strategies will encapsulate specific algorithms for handling tokens or syntax
 * within a particular state. For example, a state for parsing attributes might
 * use different strategies for handling 'id', 'class', or 'style' attributes.
 */
class Strategy {
public:
    virtual ~Strategy() = default;

    // The execution method for the strategy.
    virtual void execute(CHTLContext& context) = 0;
};

} // namespace CHTL
