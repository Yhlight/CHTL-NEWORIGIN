#ifndef CHTL_TEXT_NODE_STRATEGY_H
#define CHTL_TEXT_NODE_STRATEGY_H

#include "ParsingStrategy.h"

namespace CHTL {

class TextNodeStrategy : public ParsingStrategy {
public:
    void execute(CHTLParser& parser) override;
};

} // namespace CHTL

#endif // CHTL_TEXT_NODE_STRATEGY_H