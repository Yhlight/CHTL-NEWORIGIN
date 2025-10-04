#pragma once

#include "ParsingStrategy.h"
#include "../CHTLNode/StyleNode.h"

namespace CHTL {

void parseProperties(CHTLParserContext* context, std::shared_ptr<BaseNode> parentNode);

class StyleParsingStrategy : public ParsingStrategy {
public:
    std::shared_ptr<BaseNode> parse(CHTLParserContext* context) override;
};

}