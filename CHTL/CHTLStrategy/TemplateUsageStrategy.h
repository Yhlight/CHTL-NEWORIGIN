#ifndef CHTL_TEMPLATE_USAGE_STRATEGY_H
#define CHTL_TEMPLATE_USAGE_STRATEGY_H

#include "ParsingStrategy.h"

namespace CHTL {

class TemplateUsageStrategy : public ParsingStrategy {
public:
    std::unique_ptr<BaseNode> parse(CHTLParser& parser) override;
};

} // namespace CHTL

#endif // CHTL_TEMPLATE_USAGE_STRATEGY_H