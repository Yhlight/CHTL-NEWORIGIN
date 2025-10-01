#ifndef CHTL_IF_STRATEGY_H
#define CHTL_IF_STRATEGY_H

#include "ParsingStrategy.h"

namespace CHTL {

class IfStrategy : public ParsingStrategy {
public:
    std::unique_ptr<BaseNode> parse(CHTLParser& parser) override;
};

} // namespace CHTL

#endif // CHTL_IF_STRATEGY_H