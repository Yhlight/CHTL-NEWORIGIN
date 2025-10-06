#pragma once

#include "ParsingStrategy.h"
#include "../CHTLNode/BaseNode.h"
#include <memory>

namespace CHTL {
    class CHTLParserContext;

    class IfParsingStrategy : public ParsingStrategy {
    public:
        std::shared_ptr<BaseNode> parse(CHTLParserContext* context) override;
    };

}