#ifndef CHTL_STYLE_STRATEGY_H
#define CHTL_STYLE_STRATEGY_H

#include "ParsingStrategy.h"

namespace CHTL {

class StyleStrategy : public ParsingStrategy {
public:
    std::unique_ptr<BaseNode> parse(CHTLParser& parser) override;
};

} // namespace CHTL

#endif // CHTL_STYLE_STRATEGY_H