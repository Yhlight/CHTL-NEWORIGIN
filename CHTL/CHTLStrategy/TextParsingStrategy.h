#pragma once

#include "ParsingStrategy.h"

namespace CHTL {

class TextParsingStrategy : public ParsingStrategy {
public:
    std::shared_ptr<BaseNode> parse(CHTLParserContext* context) override;
};

}