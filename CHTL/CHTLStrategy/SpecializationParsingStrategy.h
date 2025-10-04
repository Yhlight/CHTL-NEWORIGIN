#pragma once

#include "ParsingStrategy.h"
#include "../CHTLNode/BaseNode.h"
#include <memory>

namespace CHTL {
    class CHTLParserContext;

    class SpecializationParsingStrategy : public ParsingStrategy {
    public:
        std::shared_ptr<BaseNode> parse(CHTLParserContext* context) override;
    };

}