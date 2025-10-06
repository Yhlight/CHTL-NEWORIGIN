#pragma once

#include "ParsingStrategy.h"
#include "../CHTLNode/BaseNode.h"
#include <memory>

namespace CHTL {
    class CHTLParserContext;

    enum class IfParsingMode {
        Styling,
        Rendering
    };

    class IfParsingStrategy : public ParsingStrategy {
    private:
        IfParsingMode mode;
    public:
        IfParsingStrategy(IfParsingMode mode);
        std::shared_ptr<BaseNode> parse(CHTLParserContext* context) override;
    };

}