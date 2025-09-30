#ifndef CHTL_STRATEGY_H
#define CHTL_STRATEGY_H

namespace CHTL {

class CHTLContext; // Forward declaration

class CHTLStrategy {
public:
    virtual ~CHTLStrategy() = default;
    virtual void execute(CHTLContext* context) = 0;
};

} // namespace CHTL

#endif // CHTL_STRATEGY_H