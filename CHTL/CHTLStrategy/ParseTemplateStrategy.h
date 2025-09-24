#ifndef CHTL_PARSE_TEMPLATE_STRATEGY_H
#define CHTL_PARSE_TEMPLATE_STRATEGY_H

#include "Strategy.h"

namespace CHTL {

class ParseTemplateStrategy : public Strategy {
public:
    void Execute(Context* context, Parser& parser) override;
};

} // namespace CHTL

#endif // CHTL_PARSE_TEMPLATE_STRATEGY_H
