#ifndef CHTL_SCRIPT_STRATEGY_H
#define CHTL_SCRIPT_STRATEGY_H

#include "ParsingStrategy.h"

namespace CHTL {

class ScriptStrategy : public ParsingStrategy {
public:
    std::unique_ptr<BaseNode> parse(CHTLParser& parser) override;
};

} // namespace CHTL

#endif // CHTL_SCRIPT_STRATEGY_H