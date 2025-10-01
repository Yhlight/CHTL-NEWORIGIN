#ifndef CHTL_ATTRIBUTE_STRATEGY_H
#define CHTL_ATTRIBUTE_STRATEGY_H

#include "ParsingStrategy.h"

namespace CHTL {

class AttributeStrategy : public ParsingStrategy {
public:
    void execute(CHTLParser& parser) override;
};

} // namespace CHTL

#endif // CHTL_ATTRIBUTE_STRATEGY_H