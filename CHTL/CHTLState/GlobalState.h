#ifndef CHTL_GLOBAL_STATE_H
#define CHTL_GLOBAL_STATE_H

#include "State.h"

namespace CHTL {

class GlobalState : public State {
public:
    void handle(CHTLParser& parser) override;
};

} // namespace CHTL

#endif // CHTL_GLOBAL_STATE_H