#pragma once

#include "ParsingStrategy.h"

namespace CHTL {

class ConfigurationParsingStrategy : public ParsingStrategy {
public:
    std::shared_ptr<BaseNode> parse(CHTLParserContext* context) override;
};

}