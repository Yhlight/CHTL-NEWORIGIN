#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include "../CHTLState/CHTLState.h"
#include "../CHTLStrategy/CHTLStrategy.h"

namespace CHTL {

class CHTLState;
class CHTLStrategy;

class CHTLContext {
public:
    CHTLContext(CHTLState* state, CHTLStrategy* strategy);
    ~CHTLContext();

    void setState(CHTLState* state);
    void setStrategy(CHTLStrategy* strategy);

    void request();
    void executeStrategy();

private:
    CHTLState* _state;
    CHTLStrategy* _strategy;
};

} // namespace CHTL

#endif // CHTL_CONTEXT_H