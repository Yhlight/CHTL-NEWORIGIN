#pragma once

#include "../CHTLNode/BaseNode.h"
#include <vector>
#include <memory>

namespace CHTL {
    class CHTLParserContext;

    class ParsingStrategy {
    public:
        virtual ~ParsingStrategy() = default;
        virtual std::shared_ptr<BaseNode> parse(CHTLParserContext* context) = 0;
    };
}