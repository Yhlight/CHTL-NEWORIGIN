#ifndef CHTL_STRATEGY_H
#define CHTL_STRATEGY_H

namespace CHTL {

class Context;
class Parser; // Forward-declare

class Strategy {
public:
    virtual ~Strategy() = default;
    virtual void Execute(Context* context, Parser& parser) = 0;
};

} // namespace CHTL

#endif // CHTL_STRATEGY_H
