#ifndef CHTL_GLOBAL_STATE_H
#define CHTL_GLOBAL_STATE_H

#include "IState.h"

namespace CHTL
{
    namespace State
    {
        class GlobalState : public IState
        {
        public:
            void Handle(Parser& parser) override;
        };
    }
}

#endif // CHTL_GLOBAL_STATE_H
