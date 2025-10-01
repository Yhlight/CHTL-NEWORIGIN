#ifndef CHTL_COMMENT_STRATEGY_H
#define CHTL_COMMENT_STRATEGY_H

#include "ParsingStrategy.h"

namespace CHTL {

class CommentStrategy : public ParsingStrategy {
public:
    void execute(CHTLParser& parser) override;
};

} // namespace CHTL

#endif // CHTL_COMMENT_STRATEGY_H