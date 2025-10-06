#pragma once

#include "ParsingStrategy.h"

namespace CHTL {

class TemplateParsingStrategy : public ParsingStrategy {
public:
    std::shared_ptr<BaseNode> parse(CHTLParserContext* context) override;
};

}