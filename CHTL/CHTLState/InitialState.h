#ifndef CHTL_INITIAL_STATE_H
#define CHTL_INITIAL_STATE_H

#include "ParsingState.h"

namespace CHTL {

class InitialState : public ParsingState {
public:
    void handle(CHTLParser& parser) override;
};

} // namespace CHTL

#endif // CHTL_INITIAL_STATE_H