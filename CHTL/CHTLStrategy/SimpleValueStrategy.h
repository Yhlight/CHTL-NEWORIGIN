#ifndef CHTL_SIMPLE_VALUE_STRATEGY_H
#define CHTL_SIMPLE_VALUE_STRATEGY_H

#include "IStyleValueParseStrategy.h"

namespace CHTL
{
    namespace Strategy
    {
        class SimpleValueStrategy : public IStyleValueParseStrategy
        {
        public:
            std::string Parse(Parser& parser) override;
        };
    }
}

#endif // CHTL_SIMPLE_VALUE_STRATEGY_H
