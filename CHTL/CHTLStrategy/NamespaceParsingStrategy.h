#pragma once

#include "ParsingStrategy.h"

namespace CHTL {

class NamespaceParsingStrategy : public ParsingStrategy {
public:
    std::shared_ptr<BaseNode> parse(CHTLParserContext* context) override;
};

}