#pragma once

#include "ParsingStrategy.h"
#include "../CHTLNode/AnimateNode.h"

namespace CHTL {

    class AnimateParsingStrategy : public ParsingStrategy {
    public:
        std::shared_ptr<BaseNode> parse(CHTLParserContext* context) override;
    };

}