#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include "CHTLParserContext.h"
#include <vector>
#include <memory>

namespace CHTL {

    class CHTLParser {
    public:
        CHTLParser(const std::vector<Token>& tokens);
        std::shared_ptr<BaseNode> parse();

    private:
        CHTLParserContext context;
    };

}