#ifndef CHTL_ELEMENT_STRATEGY_H
#define CHTL_ELEMENT_STRATEGY_H

#include "ParsingStrategy.h"

namespace CHTL {

class ElementStrategy : public ParsingStrategy {
public:
    void execute(CHTLParser& parser) override;
};

} // namespace CHTL

#endif // CHTL_ELEMENT_STRATEGY_H